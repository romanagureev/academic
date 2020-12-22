import math
import numpy as np


def normalized_one(obj):
  shift = obj.index(min(obj))
  return tuple(obj[shift:] + obj[:shift])


def normalized(objects):
  normalized_objects = [normalized_one(obj) for obj in objects]
  return tuple(sorted(set(normalized_objects)))


def get_face_edges(face: list):
  ''' Get edges of face generator. '''
  for i in range(len(face)):
    yield (face[i], face[(i + 1) % len(face)])
  return


def normalize(array):
  minimum = min(array)
  array = [a - minimum for a in array]
  maximum = max(array)
  if not np.isclose(maximum, 0):
    array = [a/maximum for a in array]
  return array


def heatmap(value): # value belongs to [0,1]
  return (2 * value, 2 * value, 1) if value < 1 / 2 else (1, 2 * value, 2 * value)


#---------------------------------- geometry -----------------------------------


EPS = 1e-9


def circumcenter(pt1, pt2, pt3):
  pt1, pt2, pt3 = map(np.array, (pt1, pt2, pt3))
  a = pt1 - pt3
  b = pt2 - pt3
  
  c = b * np.linalg.norm(a) ** 2 - a * np.linalg.norm(b) ** 2
  axb = np.cross(a, b)
  
  return pt3 + (np.cross(c, axb) / (2 * np.linalg.norm(axb) ** 2))


def height(pt1, pt2, pt_from):
  ''' Height from pt_from to line (pt1, pt2). '''
  base = pt1 - pt2
  base = base / np.linalg.norm(base)
  v = pt_from - pt2
  
  proj = v - np.dot(base, v) * base
  return np.linalg.norm(proj)


def area(pt1, pt2, pt3):
  a, b, c = np.linalg.norm(pt1 - pt2), np.linalg.norm(pt2 - pt3), np.linalg.norm(pt3 - pt1)
  s = (a + b + c) / 2
  return math.sqrt(s * (s - a) * (s - b) * (s - c))


def angle(pt_from, pt_to):
  norm1, norm2 = np.linalg.norm(pt_from), np.linalg.norm(pt_to)
  if math.isclose(norm1, 0, abs_tol=EPS) or math.isclose(norm2, 0, abs_tol=EPS):
    return 0
  
  cos = np.dot(pt_from, pt_to) / (norm1 * norm2)
  # Due to error, cos can be out of [-1, 1]
  if math.isclose(abs(cos), 1, abs_tol=EPS):
    cos = 1 if cos > 0 else -1

  return np.arccos(cos)


def dihedral_orientation(a, b, c, d):
    oriented = np.dot(d - a, np.cross(b - a, c - a))
    return 1 if oriented >= 0 else -1


def dihedral_angle(pt1, pt2, pt_from, pt_to):
    base = pt1 - pt2
    base = base / np.linalg.norm(base)
    v_from = pt_from - pt2
    v_to = pt_to - pt2
    
    # Projections on plane perpendicular to base
    proj_from = v_from - np.dot(base, v_from) * base
    proj_to = v_to - np.dot(base, v_to) * base
    
    short_angle = abs(angle(proj_from, proj_to))
    if dihedral_orientation(pt_to, pt1, pt2, pt_from) == 1:
        return short_angle
    else:
        return 2 * math.pi - short_angle
