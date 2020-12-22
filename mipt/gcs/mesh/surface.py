import draw

from collections import deque, defaultdict
from common import *


def is_cycle(link: tuple) -> bool:
  ''' Check:
  1) every degree = 2
  2) is connected
  '''
  vertices, edges = link
  if len(vertices) < 3:
    return False

  queue = deque([vertices[0]])
  used = set([vertices[0]])
  while len(queue) > 0:
    vertex = queue.popleft()
    degree = 0
    for adjacent_vertex in edges[vertex]:
      degree += 1
      if adjacent_vertex not in used:
        queue.append(adjacent_vertex)
        used.add(adjacent_vertex)
    if degree != 2:
      return False
  return len(used) == len(vertices)


class Surface():
  def __init__(self, faces: list, coordinates=None):
    vertices = set(i for f in faces for i in f)
    self.vertices = list(vertices)

    self.faces = faces

    self.adjacency_list = {vertex: set() for vertex in self.vertices}
    for face in self.faces:
      for edge in get_face_edges(face):
        self.adjacency_list[edge[0]].add(edge[1])
        self.adjacency_list[edge[1]].add(edge[0])

    self.V = max(vertices) + 1
    self.E = sum(map(len, self.adjacency_list.values())) // 2
    self.F = len(self.faces)
    if coordinates != None:
      self.coordinates = np.array(coordinates)

  @classmethod
  def from_obj(cls, filename):
    with open(filename,'r') as obj:
      lines = [ [f for f in s.split(' ') if len(f)>0] for s in obj.read().split('\n') ]
    vertices = [[float(coord) for coord in l[1:4]] for l in lines if len(l)>3 and l[0]=='v']
    faces = [[int(coord.split('/')[0])-1 for coord in l[1:4]] for l in lines if len(l)>3 and l[0]=='f']
    return cls(faces, vertices)

  def draw(self):
    draw.draw([list(f) for f in self.faces], self.coordinates.tolist())

  def __get_link(self, vertex: int) -> list:
    vertices = list(self.adjacency_list[vertex])
    edges = {vertex: set() for vertex in vertices}
    for face in self.faces:
      if vertex in face:
        edge = list(face)
        edge.remove(vertex)
        edges[edge[0]].add(edge[1])
        edges[edge[1]].add(edge[0])

    return (vertices, edges)

  def is_surface(self) -> bool:
    ''' Check that all links are cycles. '''
    for vertex in self.vertices:
      link = self.__get_link(vertex)
      if not is_cycle(link):
        return False
    return True

  def __count_components(self) -> int:
    ''' Using BFS. '''
    components_cnt = 0

    used = set()
    current_vertex = 0
    while len(used) < self.V:
      while current_vertex in used:
        current_vertex += 1

      queue = deque([current_vertex])
      used.add(current_vertex)

      while len(queue) > 0:
        next_vertex = queue.popleft()
        for candidate in self.adjacency_list[next_vertex]:
          if candidate not in used:
            used.add(candidate)
            queue.append(candidate)

      components_cnt += 1

    return components_cnt

    
  def is_connected(self) -> bool:
    ''' Check if ther is only 1 component. '''
    return self.__count_components() == 1
  
  def __edge_hash(self, edge: tuple) -> int:
    ''' Hash of edge without intersections. '''
    start, end = edge
    if start > end:
      return end * self.V + start
    else:
      return start * self.V + end

  def is_oriented(self) -> bool:
    ''' Check that each edge is used in both directions once. '''
    oriented_edges = {vertex: set() for vertex in self.vertices}
    for face in self.faces:
      for edge in get_face_edges(face):
        oriented_edges[edge[0]].add(edge[1])
    edge_counter = defaultdict(int)

    for start in self.vertices:
      for end in oriented_edges[start]:
        edge_counter[self.__edge_hash((start, end))] += 1 if start < end else self.V

    for count in edge_counter.values():
      if count != self.V + 1:
        return False

    return True

  def __get_dual(self) -> list:
    edges_to_face = defaultdict(int)
    for i, face in enumerate(self.faces):
      for edge in get_face_edges(face):
        edge_hash = self.__edge_hash(edge)
        edges_to_face[edge_hash] = max(edges_to_face[edge_hash], i)

    dual_graph = {i: [] for i in range(self.F)}
    for i, face in enumerate(self.faces):
      for edge in get_face_edges(face):
        adjacent_face = edges_to_face[self.__edge_hash(edge)]
        if adjacent_face != i:
          dual_graph[i].append(adjacent_face)
          dual_graph[adjacent_face].append(i)

    return dual_graph

  def is_orientable(self) -> bool:
    ''' Trying to orient with BFS order of dual graph. '''
    dual_graph = self.__get_dual()
    edges = set()
    used = set([0])
    current_face_index = 0
    while len(used) < self.V:
      while current_face_index in used:
        current_face_index += 1

      queue = deque([current_face_index])
      used.add(current_face_index)
      while len(queue) > 0:
        face_index = queue.popleft()
        face = self.faces[face_index]
        straight_orientation = False
        reversed_orientation = False

        for edge in get_face_edges(face):
          if edge in edges:
            reversed_orientation = True
        for edge in get_face_edges(face[::-1]):
          if edge in edges:
            straight_orientation = True

        if straight_orientation and reversed_orientation:
          return False

        face_to_add = face[::-1] if reversed_orientation else face
        for edge in get_face_edges(face_to_add):
          edges.add(edge)

        for adjacent_face in dual_graph[face_index]:
          if adjacent_face not in used:
            used.add(adjacent_face)
            queue.append(adjacent_face)
    return True
              
  def Euler(self) -> int:
    ''' Get Euler characteristic. '''
    return self.V - self.E + self.F

  def angle_defects(self):
    angles_sum = {i: 0 for i in range(self.V)}
    for face in self.faces:
      for i, vertex in enumerate(face):
        A, B, C = self.coordinates[face[i - 1]], self.coordinates[vertex], self.coordinates[face[(i + 1) % 3]]
        angles_sum[vertex] += abs(angle(A - B, C - B))

    defects = []
    for angle_sum in angles_sum.values():
      defects.append(2 * math.pi - angle_sum)

    return defects
  
  def volume(self):
    ''' Assume that surface is oriented. '''

    oriented_volume = 0
    for face in self.faces:
      A, B, C = self.coordinates[face[0]], self.coordinates[face[1]], self.coordinates[face[2]]
      oriented_volume += np.dot(A, np.cross(B, C)) / 6

    return oriented_volume

  def mean_curvature_flow(self):
    flow = {i: np.array([0., 0., 0.]) for i in range(self.V)}
    for face in self.faces:
      for i in range(len(face)):
        vertex = face[i]
        A, B, C = self.coordinates[face[i - 1]], self.coordinates[vertex], self.coordinates[face[(i + 1) % 3]]

        tan = math.tan(angle(A - C, B - C))
        if not math.isclose(tan, 0, abs_tol=EPS):
          flow[vertex] += (B - A) * (1 / (2 * tan))
        tan = math.tan(angle(C - A, B - A))
        if not math.isclose(tan, 0, abs_tol=EPS):
          flow[vertex] += (B - C) * (1 / (2 * tan))

    return list(flow.values())
  
  def __edge_hash(self, edge: tuple) -> int:
    ''' Hash of edge without intersections. '''
    start, end = edge
    if start > end:
      return end * self.V + start
    else:
      return start * self.V + end
  
  def __edges_to_faces(self):
    ''' Assume that faces are oriented. '''
    edges_to_faces = defaultdict(lambda: [None, None])
    for i, face in enumerate(self.faces):
      for edge in get_face_edges(face):
        normalized_edge = tuple(sorted(edge))
        # Using orientation.
        if edge == normalized_edge:
          edges_to_faces[normalized_edge][0] = i
        else:
          edges_to_faces[normalized_edge][1] = i
            
    for edge, faces in edges_to_faces.items():
      assert len(faces) == 2

    return edges_to_faces
  
  def schlafli(self, flow):
    edges_to_faces = self.__edges_to_faces()
    
    schlafli_sum = 0
    for edge, faces in edges_to_faces.items():
      pt1, pt2 = self.coordinates[edge[0]], self.coordinates[edge[1]]
      pt_from_i = [v for v in self.faces[faces[0]] if v not in edge][0]
      pt_to_i = [v for v in self.faces[faces[1]] if v not in edge][0]
      pt_from, pt_to = self.coordinates[pt_from_i], self.coordinates[pt_to_i]
      
      t = 1e-3
      diff = dihedral_angle(pt1 + t * flow[edge[0]],
                            pt2 + t * flow[edge[1]],
                            pt_from + t * flow[pt_from_i],
                            pt_to + t * flow[pt_to_i]) - \
              dihedral_angle(pt1, pt2, pt_from, pt_to)
      edge_len = np.linalg.norm(pt1 - pt2)
      schlafli_sum += edge_len * (diff / t)

    return schlafli_sum
