from collections import defaultdict
from common import *

class DualMeshCreator():
  @staticmethod
  def add_edges(faces, obj):
    edges_to_face = defaultdict(list)
    for i, face in enumerate(faces):
      for edge in get_face_edges(face):
        edges_to_face[normalized_one(edge)].append(i)

    n_edges_to_face = dict()
    for edge, dual_edge in edges_to_face.items():
      n_edges_to_face[edge] = normalized_one(dual_edge)
    obj.dual_edges = normalized(n_edges_to_face.values())
    obj.edge_to_dual = n_edges_to_face
    obj.dual_to_edge = {dual: edge for edge, dual in n_edges_to_face.items()}

  @staticmethod
  def __orient_dual_edge(v, to, dual_edge, faces):
    for edge in get_face_edges(faces[dual_edge[0]]):
      if v not in edge:
        if to == edge[0]:
          # reverse
          dual_edge = dual_edge[::-1]
        break
    return dual_edge

  @staticmethod
  def __links(V, edges):
    links = [[] for _ in range(V)]
    for i, edge in enumerate(edges):
      links[edge[0]].append((edge[1], i))
      links[edge[1]].append((edge[0], i))
    return links

  @staticmethod
  def __other(face, v1, v2):
    for v in face:
      if v != v1 and v != v2:
        return v

  @staticmethod
  def __other_in_edge(edge, v):
    if v == edge[0]:
      return edge[1]
    else:
      return edge[0]

  @staticmethod
  def make_faces(V, edges, faces, dual_edges, edge_to_dual):
    ''' Returns oriented dual faces. '''
    links = DualMeshCreator.__links(V, edges)

    dual_faces = []
    for v in range(V):
      # first edge - first edge in link
      first_edge = edges[links[v][0][1]]
      to = links[v][0][0]
      dual_edge = DualMeshCreator.__orient_dual_edge(
        v, to, edge_to_dual[first_edge], faces
      )

      dual_face = [dual_edge[0], dual_edge[1]]
      link = dict(links[v])
      # Add other dual vertices in chosen order
      for _ in range(len(link) - 2):
        to = DualMeshCreator.__other(faces[dual_face[-1]], v, to)
        dual_edge = edge_to_dual[normalized_one((v, to))]
        dual_face.append(DualMeshCreator.__other_in_edge(dual_edge, dual_face[-1]))

      dual_faces.append(dual_face)
    return dual_faces
