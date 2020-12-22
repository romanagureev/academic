import matrix
import unittest


class TestMatrix(unittest.TestCase):
  def setUp(self):
    self.E = [[1, 0], [0, 1]]
    self.A = [[1, 2], [3, 4]]
    self.B = [[1, 2], [1, 2]]

  def test_id(self):
    result = matrix.dot(2, self.E, self.B)
    correct = self.B
    for r, s in zip(result, correct):
      for x, y in zip(r, s):
        self.assertEqual(x, y)

  def test_AB(self):
    result = matrix.dot(2, self.A, self.B)
    correct = [[3, 6], [7, 14]]
    for r, s in zip(result, correct):
      for x, y in zip(r, s):
        self.assertEqual(x, y)

  def test_less(self):
    Less = [[1]]
    result = matrix.dot(2, self.A, Less)
    correct = [[1, 0], [3, 0]]
    for r, s in zip(result, correct):
      for x, y in zip(r, s):
        self.assertEqual(x, y)

  def test_less(self):
    More = [[1, 2, 3], [4, 5, 6]]
    result = matrix.dot(2, self.A, More)
    correct = [[9, 12], [19, 26]]
    for r, s in zip(result, correct):
      for x, y in zip(r, s):
        self.assertEqual(x, y)


if __name__ == '__main__':
  unittest.main()
