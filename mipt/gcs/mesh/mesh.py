import copy
import draw # if you don't use pythreejs, remove this import and Mesh.draw()
import scipy.sparse.linalg
import time

from collections import defaultdict
from common import *
from scipy.sparse import dok_matrix, csc_matrix
from dual_mesh import DualMeshCreator


class Mesh():
  def __init__(self, faces, coordinates=None):
    self.faces = [tuple(face) for face in faces]

    edges = []
    for i, face in enumerate(self.faces):
      for edge in get_face_edges(face):
        edges.append(edge)
    self.edges = normalized(edges)

    vertices = set(i for f in faces for i in f)
    self.V = max(vertices) + 1
    self.E = len(self.edges)
    self.F = len(self.faces)
    if coordinates != None:
      self.coordinates = np.array(coordinates)
      self.dual_coordinates = [circumcenter(coordinates[i], coordinates[j], coordinates[k])
                               for (i, j, k) in self.faces]

    assert set(range(self.V)) == vertices
    for f in faces:
      assert len(f) == 3
    if coordinates != None:
      assert self.V == len(coordinates)
      for c in coordinates:
        assert len(c) == 3

    DualMeshCreator.add_edges(self.faces, self)
    self.dual_faces = DualMeshCreator.make_faces(
      self.V, self.edges, self.faces, self.dual_edges, self.edge_to_dual
    )  # Index of face = center vertex

    assert len(self.dual_faces) == self.V
    assert len(self.dual_edges) == self.E
    
  @classmethod
  def from_obj(cls, filename):
    with open(filename,'r') as obj:
      lines = [ [f for f in s.split(' ') if len(f)>0] for s in obj.read().split('\n') ]
    vertices = [[float(coord) for coord in l[1:4]] for l in lines if len(l)>3 and l[0]=='v']
    faces = [[int(coord.split('/')[0])-1 for coord in l[1:4]] for l in lines if len(l)>3 and l[0]=='f']
    return cls(faces, vertices)

  def draw(self):
    draw.draw(self.faces, self.coordinates.tolist())
      
  def save_obj(self, filename: str, function=None):
    if function != None:
      function = normalize(function)
    with open(filename, 'w') as obj:
      if function != None:
        for v, f in zip(self.coordinates, function):
          obj.write(f"v {' '.join(str(c) for c in v)} {' '.join(str(c) for c in heatmap(f))}\n")
      else:
        for v in self.coordinates:
          obj.write(f"v {' '.join(str(c) for c in v)}\n")
      for f in self.faces:
        obj.write(f"f {' '.join(str(c+1) for c in f)}\n")

  def __edge_index(self, edge):
    return self.edges.index(normalized_one(edge))

  def __dual_edge_index(self, dual_edge):
    return self.dual_edges.index(normalized_one(dual_edge))

  def __edge_mul(self, edge):
    return 1 if edge == normalized_one(edge) else -1
      
  def d(self, k, form, dual=False):
    """ Exterior derivative of k-form. """
    d_form = []
    if k == 0:
      if dual:
        for dual_edge in self.dual_edges:
          d_form.append(form[dual_edge[1]] - form[dual_edge[0]])
      else:
        for edge in self.edges:
          d_form.append(form[edge[1]] - form[edge[0]])

    elif k == 1:
      if dual:
        for face in self.dual_faces:
          value = 0
          for edge in get_face_edges(face):
            value += form[self.__dual_edge_index(edge)] * self.__edge_mul(edge)
          d_form.append(value)
      else:
        if dual:
          for dual_face in self.dual_faces:
            value = 0
            for dual_edge in get_face_edges(dual_face):
              value += form[self.__dual_edge_index(dual_edge)] * self.__edge_mul(dual_edge)
            d_form.append(value)
        else:
          for face in self.faces:
            value = 0
            for edge in get_face_edges(face):
              value += form[self.__edge_index(edge)] * self.__edge_mul(edge)
            d_form.append(value)

    elif k == 2:
      d_form = [0]

    else:
      raise ValueError(f'{k} is a bad form degree.')

    return np.array(d_form)

  def edges_order(self):
    ''' Order of edges in 1-form. '''
    return self.edges

  def dual_edges_order(self):
    ''' Order of dual edges in dual 1-form. '''
    return self.dual_edges

  def wedge(self, k1, k2, form1, form2):
    """ Exterior product of k1-form1 and k2-form2. """
    if k2 > k1:
      k1, k2 = k2, k1
      form1, form2 = form2, form1

    form = []
    if k1 == 0 and k2 == 0:
      form = list(np.array(form1) * np.array(form2))

    elif k1 == 1 and k2 == 0:
      for i, edge in enumerate(self.edges):
        form.append(form1[i] * (form2[edge[0]] + form2[edge[1]]) / 2)

    elif k1 == 2 and k2 == 0:
      for i, face in enumerate(self.faces):
        form.append(form1[i] * (form2[face[0]] + form2[face[1]] + form2[face[2]]) / 3)

    elif k1 == 1 and k2 == 1:
      for face in self.faces:
        edges_indexes = [self.__edge_index(edge) for edge in get_face_edges(face)]
        edges_mul = [self.__edge_mul(edge) for edge in get_face_edges(face)]

        value = 0
        for i in range(3):
          j = (i + 1) % 3

          e_i = edges_indexes[i]
          e_j = edges_indexes[j]


          mul = edges_mul[i] * edges_mul[j]
          value += (form1[e_i] * form2[e_j] - form1[e_j] * form2[e_i]) * mul

        form.append(value / 6)

    else:
      raise ValueError(f'{k1, k2} are bad form degrees.')

    return form

  def __edge_length(self, i: int, j: int, dual=False):
    if dual:
      edge = self.dual_to_edge[(i, j)]
      base1, base2 = self.coordinates[edge[0]], self.coordinates[edge[1]]
      return height(base1, base2, self.dual_coordinates[i]) + \
              height(base1, base2, self.dual_coordinates[j])

    return np.linalg.norm(self.coordinates[i] - self.coordinates[j])

  def dual_area(self, i: int):
    ''' Area of i-th dual face. '''
    area = 0
    for face in self.dual_faces[i]:
      vertices = list(copy.copy(self.faces[face]))
      vertices.remove(i)
      a, b = vertices
      a_c, b_c, i_c = self.coordinates[a], self.coordinates[b], self.coordinates[i]
      
      area += self.__edge_length(i, a) ** 2 / math.tan(angle(a_c - b_c, i_c - b_c))
      area += self.__edge_length(i, b) ** 2 / math.tan(angle(b_c - a_c, i_c - a_c))

    return area / 8

  def area(self, i: int):
    ''' Area of i-th face. '''
    a, b, c = self.faces[i]
    return area(self.coordinates[a], self.coordinates[b], self.coordinates[c])

  def star(self, k: int, form, dual=False):
    """ Hodge star of k-form. """
    result = []
    if k == 0:
      if dual:
        for i in range(len(self.faces)):
          result.append(self.area(i) * form[i])
      else:
        for i in range(len(self.dual_faces)):
          result.append(self.dual_area(i) * form[i])

    elif k == 1:
      if dual:
        for edge in self.edges:
          dual_edge = self.edge_to_dual[edge]
          coef = self.__edge_length(edge[0], edge[1]) / self.__edge_length(dual_edge[0], dual_edge[1], True)
          result.append(coef * form[self.__dual_edge_index(dual_edge)])
      else:
        for dual_edge in self.dual_edges:
          edge = self.dual_to_edge[dual_edge]
          coef = self.__edge_length(dual_edge[0], dual_edge[1], True) / self.__edge_length(edge[0], edge[1])
          result.append(coef * form[self.__edge_index(edge)])

    elif k == 2:
      if dual:
        for i in range(len(self.dual_faces)):
          result.append(form[i] / self.dual_area(i))
      else:
        for i in range(len(self.faces)):
          result.append(form[i] / self.area(i))

    else:
      raise ValueError(f'Bad k: {k}')

    return np.array(result)

  def cod(self, k: int, form):
    ''' Codifferential of k-form. '''
    if k == 0:
      return np.array([0])

    s = self.star(k, form)
    ds = self.d(2 - k, s, dual=True)
    sds = self.star(2 - k + 1, ds, dual=True)
    return sds

  def __laplace_codd_dcod(self, k: int, form):
    if k == 0:
      return self.cod(k + 1, self.d(k, form))
    elif k == 2:
      return self.d(k - 1, self.cod(k, form))

    return self.cod(k + 1, self.d(k, form)) + self.d(k - 1, self.cod(k, form))


  def M(self):
    ''' Mass matrix. '''
    return np.array([self.dual_area(i) for i in range(self.V)])


  def weak_laplace_operator(self): 
    '''
    A weak Laplacian matrix C that consists of cotangents.
    '''
    matrix = dok_matrix((self.V, self.V))
    for face in self.faces:
      for i in range(3):
        a, b, c = face[i], face[i - 2], face[i - 1]
        A = self.coordinates[a]
        B, C = self.coordinates[b], self.coordinates[c]
        cot = 1 / math.tan(abs(angle(B - A, C - A)))
        matrix[b, c] += cot
        matrix[c, b] += cot
        matrix[b, b] -= cot
        matrix[c, c] -= cot
    return matrix / 2


  def laplace_operator(self):
    ''' Laplace operator for 0-form. '''
    M_1 = dok_matrix((self.V, self.V))
    M_1.setdiag(1 / self.M())
    C = self.weak_laplace_operator()
    return M_1.dot(C)


  def laplacian(self, k: int, form, method='Definition'):
    ''' Laplacian of k-form
    methods:
      Definition - sdsd + dsds
      Weak - M^{-1}C, where M - mass matrix, C - weak laplace operator. Only for 0-forms.
    '''
    if method == 'Definition':
      return self.__laplace_codd_dcod(k, form)
    elif method == 'Weak' and k == 0:
      return self.laplace_operator().dot(np.array(form))

    else:
      raise ValueError(f'Unkown method to find laplace operator of {k}-form: {method}.')

      
  def heat_flow(self, func, step=1/1000, nsteps=1000, implicit=False):
    '''
    this method integrates the function 'func' via the heat equation
    func is an array of function values at vertices,
    step is the length of one step (also called h, or epsilon),
    nsteps is the number of steps to perform.
    implicit is an indication whether we should use implicit integration (backward Euler) or explicit (forward Euler)
    '''
    func = np.array(func, dtype=float)
    if implicit:
      M = dok_matrix((self.V, self.V))
      M.setdiag(self.M())

      C = self.weak_laplace_operator()
      A = (M - C * step).tocsc()

      for _ in range(nsteps):
        func = scipy.sparse.linalg.spsolve(A, M.dot(func))
    else:
      L = self.laplace_operator()
      for _ in range(nsteps):
        func += L.dot(func) * step
    return func
  

  def reconstruct(
      self,
      anchors,
      anchor_coordinates,
      anchor_weight=1.,
      smoothing_coefficient=None,
      log_time=False,
      **solver_args,
  ):
    '''
    anchors is a list of vertex indices, 
    anchor_coordinates is a list of same length of vertex coordinates (arrays of length 3), 
    anchor_weight is a positive number
    smoothing_coefficient is an optional function applied to differential coordinates: delta = delta*smoothing_coefficient(|delta|).
    '''
    start = time.process_time()

    L = self.laplace_operator().todok()
    A = dok_matrix((self.V + len(anchors), self.V))
    for key in L.keys():
      A[key] = L[key]

    for i, anchor in enumerate(anchors):
      A[self.V + i, anchor] = anchor_weight

    new_coordinates = []
    for coordinate in range(3):
      original_v = np.array([vertex[coordinate] for vertex in self.coordinates])
      delta = L.dot(original_v)
      if smoothing_coefficient is not None:
        delta *= smoothing_coefficient(np.linalg.norm(delta))

      delta = np.concatenate((
        delta,
        [anchor_c[coordinate] * anchor_weight for anchor_c in anchor_coordinates]
      ))

      new_coordinates.append(scipy.sparse.linalg.lsqr(A, np.array(delta), x0=original_v, **solver_args)[0])

    self.coordinates = []
    for x, y, z in zip(new_coordinates[0], new_coordinates[1], new_coordinates[2]):
      self.coordinates.append([x, y, z])

    if log_time:
      print(f'Reconstructed in {time.process_time() - start:.4}sec.')
