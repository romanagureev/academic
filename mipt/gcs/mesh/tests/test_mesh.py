import copy
import unittest
import math
import numpy as np
import sys

from datasets.varieties import arrays
from mesh import Mesh as Surface


def makeform(k, f, X):
  if k == 0:
    return np.array([f(i) for i in range(X.V)])
  elif k == 1:
    return np.array([f(e[0], e[1]) for e in X.edges])
  elif k == 2:
    return np.array([f(face[0],face[1],face[2]) for face in X.faces])


class TestMesh(unittest.TestCase):
  def setUp(self):
    self.EPS = 1e-9

    self.varieties = ['sphere', 'torus', 'icosahedron', 'sphere with two handles', 'pyramid', 'klein bottle']
    self.oriented_varieties = ['sphere', 'torus', 'icosahedron', 'sphere with two handles', 'pyramid']
    self.variety = lambda key: Surface(arrays[key])

    self.torus = self.variety('torus')
    self.sphere = self.variety('sphere')
    self.icosahedron = self.variety('icosahedron')

    self.zeroform1 = lambda u: u
    self.zeroform2 = lambda u: u**2
    self.zeroform3 = lambda v : v * 3 - v * v
    self.oneform0 = lambda u,v: 0
    self.oneform1 = lambda u,v: u ** 2 - v ** 2
    self.oneform2 = lambda u, v: u-v 
    sign = lambda x: (1 if x>0 else -1) if abs(x)>0.00001 else 0
    self.twoform0 = lambda u, v, w: sign((u-v)*(v-w)*(w-u))
    self.twoform1 = lambda x,y,z : x+y+z if (x-y)*(y-z)*(z-x)>0 else -(x+y+z)
    self.twoform3 = lambda u, v, w: (u-v)*(v-w)*(w-u)

    self.tetrahedron = Surface([(3,2,1), (2,3,0), (1,0,3),(0,1,2)], [[0,0,0], [1,0,0], [1/2, math.sqrt(3)/2, 0], [1/2, 1/2/math.sqrt(3), math.sqrt(2/3)]])


  def test_wedge00(self):
      f0 = makeform(0, lambda x: x, self.sphere)
      z1 = makeform(0, self.zeroform1, self.sphere)
      z2 = makeform(0, self.zeroform2, self.sphere)
      self.assertAlmostEqual(self.sphere.wedge(0, 0, f0, f0)[2], 4)
      for i in range(4):
        self.assertAlmostEqual(self.sphere.wedge(0, 0, z1, z2)[i], i ** 3)

  def test_wedge01(self):
      f0 = makeform(0, lambda x: x**2, self.sphere)
      f1 = makeform(1, lambda x,y: y-x, self.sphere)
      for i in range(4):
          self.assertAlmostEqual(self.sphere.wedge(0, 1, f0, f1)[i], (self.sphere.edges[i][1] - self.sphere.edges[i][0]) * (self.sphere.edges[i][0]**2 + self.sphere.edges[i][1]**2) / 2, msg=f'edge {i}={self.sphere.edges[i]}')

  def test_wedge02(self):
    z1 = makeform(0, self.zeroform1, self.sphere)
    t0 = makeform(2, self.twoform0, self.sphere)
    t1 = makeform(2, self.twoform1, self.sphere)
    
    self.assertAlmostEqual(self.sphere.wedge(0,2, z1, t1)[3], 3)
    self.assertAlmostEqual(self.sphere.wedge(0, 2, z1, t0)[2], -4 / 3)
    self.assertAlmostEqual(self.sphere.wedge(0, 2, z1, t0)[0], -2.0)
    
  def test_wedge11(self):
    o1 = makeform(1, self.oneform1, self.sphere)
    o2 = makeform(1, self.oneform2, self.sphere)
    for m, f in enumerate(arrays['sphere']):
        i, j, k = f[0], f[1], f[2]
        self.assertAlmostEqual(self.sphere.wedge(1, 1, o1, o2)[m], 1 / 6 * np.linalg.det([[1, 1, 1],
                      [i ** 2 - j ** 2, j ** 2 - k ** 2, k ** 2 - i ** 2],
                      [i - j, j - k, k - i]]), msg = f'face {m}')
    self.assertAlmostEqual(self.sphere.wedge(1, 1, o1, o2)[0], 1.0)

  def test_d0(self):
      z1 = makeform(0, self.zeroform1, self.sphere)
      self.assertAlmostEqual(self.sphere.d(0, z1)[0], self.sphere.edges[0][1]-self.sphere.edges[0][0])
    
  def test_d1(self):
      o1 = makeform(1, lambda x, y : x-y, self.sphere)
      o2 = makeform(1, lambda x, y : x*y if x<y else -x*y, self.sphere)
      self.assertAlmostEqual(self.sphere.d(1, o1)[2], 0)
      self.assertAlmostEqual(self.sphere.d(1, o2)[0], -5)

  def test_d(self):
    V = self.variety('icosahedron')
    f = makeform(0, self.zeroform3, V)
    df = V.d(0, f)
    ddf = V.d(1, df)
    self.assertAlmostEqual(ddf[4], 0)

  def test_dual_area(self):
    self.assertAlmostEqual(self.tetrahedron.dual_area(2), math.sqrt(3)/4)

  def test_star(self):
    T = self.tetrahedron
    f0 = [1]*4
    f1 = [1]*6
    self.assertAlmostEqual(T.star(0, [1,2,3,4])[2], 3*math.sqrt(3)/4)
    self.assertAlmostEqual(T.star(0, f0)[2], math.sqrt(3)/4)
    self.assertAlmostEqual(T.star(1, f1)[2], math.sqrt(3)/3)
    self.assertAlmostEqual(T.star(2, f0)[2], 4/math.sqrt(3))
    self.assertAlmostEqual(T.star(2, f0, dual=True)[2], 4/math.sqrt(3))
    self.assertAlmostEqual(T.star(1, f1, dual=True)[2], 3/math.sqrt(3))
    self.assertAlmostEqual(T.star(0, f0, dual=True)[2], math.sqrt(3)/4)
    f = [1,1,1,1] # TODO: correct f or asserts
    df = T.d(0, f)
    sdf = T.star(1, df)
    dsdf = T.d(1, sdf, dual=True)
    sdsdf = T.star(2, dsdf, dual=True)
    for entry in sdsdf:
      self.assertAlmostEqual(entry, 0)

  def test_laplacian_equivalence(self):
    T = self.tetrahedron
    f = [-1,1,-1,1]

    sdsdf = T.laplacian(0, f, method='Definition')
    M_1Cf = T.laplacian(0, f, method='Weak')

    for definition, weak in zip(sdsdf, M_1Cf):
      self.assertAlmostEqual(definition, weak)

  def test_heat_flow_decrease(self):
    T = self.tetrahedron
    f = [-1,1,-1,1]

    h_explicit = T.heat_flow(f, implicit=False)

    for fi, hi in zip(f, h_explicit):
      self.assertGreater(abs(fi), abs(hi))
      self.assertGreater(fi * (hi + self.EPS), 0)  # Of one sign

    h_implicit = T.heat_flow(f, implicit=True)

    for fi, hi in zip(f, h_implicit):
      self.assertGreater(abs(fi), abs(hi))
      self.assertGreater(fi * (hi + self.EPS), 0)  # Of one sign

  def test_heat_flow_converge(self):
    T = self.tetrahedron
    f = [-1,1,-1,1]

    h_explicit = T.heat_flow(f, step=1/100, nsteps=1000, implicit=False)
    h_implicit = T.heat_flow(f, step=1/100, nsteps=1000, implicit=True)

    for h in h_explicit:
      self.assertAlmostEqual(h, 0)

    for h in h_implicit:
      self.assertAlmostEqual(h, 0)

  def test_reconstruct_id(self):
    T = copy.deepcopy(self.tetrahedron)

    T.reconstruct([0], [T.coordinates[0]])

    for a, b in zip(T.coordinates, self.tetrahedron.coordinates):
      for a_c, b_c in zip(a, b):
        self.assertAlmostEqual(a_c, b_c)

  def test_reconstruct_move(self):
    T = copy.deepcopy(self.tetrahedron)

    shift = np.array([13, 14, -15])
    xyz = T.coordinates[0]
    T.reconstruct([0], [np.array(T.coordinates[0]) + shift])

    for a, b in zip(T.coordinates, self.tetrahedron.coordinates):
      for a_c, b_c in zip(np.array(a) - shift, b):
        self.assertAlmostEqual(a_c, b_c)


if __name__ == '__main__':
  unittest.main()
  