import math
import numpy as np

from mesh import Mesh

def perform():
  def get_close(figure: Mesh, roots: list,  neighborhood=6):
    vertices = set()
    q = set(roots)
    for _ in range(neighborhood):
      for e in figure.edges:
        if e[0] in q or e[1] in q:
          q.add(e[0])
          q.add(e[1])
      vertices.update(q)
    return vertices

  dataset = 'teddy.obj'
  teddy = Mesh.from_obj('datasets/' + dataset)

  right_arm_top = 585
  right_arm_elbow = 526
  left_arm_top = 680
  left_arm_elbow = 627
  right_arm = get_close(teddy, [right_arm_top])
  print(f'Right arm size = {len(right_arm)}')
  # left_arm = get_close(teddy, [left_arm_top])
  # print(f'Left arm size = {len(left_arm)}')

  fix = [i for i in range(teddy.V) if (i not in right_arm)]
  fixed = [teddy.coordinates[i] for i in fix]

  move = [
    right_arm_top,
    # right_arm_elbow,
    # left_arm_top,
    # left_arm_elbow,
  ]
  shifts = [
    [12, 0, 8],
    # [12, -4, 8],
    # [-15, 7, 12],
    # [-12, 4, 8],
  ]
  moved = []
  for pt, shift in zip(move, shifts):
    moved.append(np.array(teddy.coordinates[pt]) + np.array(shift))

  teddy.reconstruct(fix + move, fixed + moved, anchor_weight=0.8, log_time=True) # iter_lim for less time

  teddy.save_obj('results/' + dataset)


def dragon(): 
  mesh = Mesh.fromobj("dragon.obj")
  raise NotImplementedError


if __name__ == '__main__':
	perform()
