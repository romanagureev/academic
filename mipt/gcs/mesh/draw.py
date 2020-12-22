from pythreejs import *
from IPython.display import display


def draw(faces, vertices):
    # Create the geometry:
    vertexcolors=['#0000ff' for v in vertices]
    faces = [f + [None, [vertexcolors[i] for i in f], None] for f in faces]
    geometry = Geometry(faces=faces,vertices=vertices,colors=vertexcolors)
    # Calculate normals per face, for nice crisp edges:
    geometry.exec_three_obj_method('computeFaceNormals')

    object1 = Mesh(
        geometry=geometry,
        material=MeshLambertMaterial(color= "brown", side="FrontSide"),
    )

    object2 = Mesh(
        geometry=geometry,
        material=MeshLambertMaterial(color= "black", side="BackSide"),
    )

# Set up a scene and render it:
    camera = PerspectiveCamera(position=[2*max(v[0] for v in vertices), 2*max(v[1] for v in vertices), 2*max(v[2] for v in vertices)], fov=40,
                      children=[DirectionalLight(color='#cccccc', position=[-3, 5, 1], intensity=0.5)])
    scene = Scene(children=[object1, object2, camera, AmbientLight(color='#dddddd')])

    renderer = Renderer(camera=camera, background='black', background_opacity=1,
                        scene=scene, controls=[OrbitControls(controlling=camera)])

    display(renderer)
