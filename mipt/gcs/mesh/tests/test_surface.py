import unittest
import math
import numpy as np
from inspect import signature
import sys


from surface import Surface
from datasets.varieties import arrays


class TestSurface(unittest.TestCase):
  def setUp(self):
    self.varieties = ['sphere', 'torus', 'icosahedron', 'sphere with two handles', 'pyramid', 'klein bottle']
    self.oriented_varieties = ['sphere', 'torus', 'icosahedron', 'sphere with two handles', 'pyramid']
    self.variety = lambda key: Surface(arrays[key])


  def test_surface(self):
    for surface in self.varieties:
      self.assertTrue(self.variety(surface).is_surface(),f'error on a surface {surface}')

    # duplicate faces
    self.assertFalse( Surface([(1, 2, 3), (2, 3, 1), (3, 0, 1), (0, 1, 2)]).is_surface() )
    self.assertFalse( Surface([(1, 2, 0), (1, 0, 2)]).is_surface() )
    
    self.assertFalse(Surface([(1, 2, 0), (1, 2, 3), (1, 2, 4)]).is_surface())
    
    self.assertFalse(self.variety('two pyramids').is_surface())
    self.assertTrue(self.variety('klein bottle').is_surface())

    self.assertFalse(Surface([(0, 1, 2), (0, 2, 3)]).is_surface()) # square on a plane
    self.assertFalse(Surface([(1, 0, 2), (2, 0, 3), (3, 0, 4), (0, 1, 4), (1, 2, 4)]).is_surface()) # pyramid with a missing face

  def test_connected(self):
    for surface in self.varieties:
      self.assertTrue(self.variety(surface).is_connected(), f'error on a surface {surface}')
  
    self.assertFalse( Surface([(3, 2, 1), (2, 3, 0), (1, 0, 3), (0, 1, 2), (6, 5, 4), (5, 6, 7), (4, 7, 6), (7, 4, 5)]).is_connected() ) # two disconnected tetrahedrons

  def test_oriented(self):
    self.assertFalse(Surface([(1, 2, 3), (0, 3, 2), (3, 0, 1), (0, 1, 2)]).is_oriented()) 
    self.assertFalse(Surface([(1, 2, 3), (2, 3, 0), (3, 0, 1), (0, 1, 2)]).is_oriented())
    self.assertFalse(self.variety('klein bottle').is_oriented())
    for surface in self.oriented_varieties:
      self.assertTrue(self.variety(surface).is_oriented(), f'error on a surface {surface}')
  
    # break orientation of one face
    for surface in self.varieties:
      array = arrays[surface].copy()
      pos = len(array) // 2
      array[pos] = (array[pos][1], array[pos][0], array[pos][2])
      self.assertFalse(Surface(array).is_oriented(), 'error on a surface {} with a swapped face'.format(surface))

  def test_orientable(self):
    for surface in self.oriented_varieties:
      self.assertTrue(self.variety(surface).is_orientable(), f'error on a surface {surface}')
    self.assertTrue( Surface([(1, 2, 3), (2, 3, 0), (3, 0, 1), (0, 1, 2)]).is_orientable())
    self.assertFalse(self.variety('klein bottle').is_orientable())

  def test_Euler(self):
    self.assertEqual(self.variety('sphere').Euler(), 2)
    self.assertEqual(self.variety('torus').Euler(), 0)
    self.assertEqual(self.variety('klein bottle').Euler(), 0)
    self.assertEqual(self.variety('icosahedron').Euler(), 2)
    self.assertEqual(self.variety('sphere with two handles').Euler(), -2)


if __name__ == '__main__':
  unittest.main()
