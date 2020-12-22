from dataclasses import dataclass, field
from typing import List
import math
import numpy as np
from enum import Enum


EPS = 1e-12


@dataclass
class Point:
    x: float
    y: float
    def __add__(self, other): 
        return Point(x = self.x + other.x, y = self.y + other.y)
    def __sub__(self, other): 
        return Point(x = self.x - other.x, y = self.y - other.y)
    def __neg__(self):
        return Point(-self.x, -self.y)
    def __rmul__(self, scalar):
        return Point(x = self.x*scalar, y = self.y*scalar)
    def __truediv__(self, scalar):
        return Point(self.x / scalar, self.y / scalar)
    def __eq__(self, other):
        return math.isclose(self.x, other.x, abs_tol = EPS) and  \
               math.isclose(self.y, other.y, abs_tol = EPS)
    def __lt__(self, other):
        return self.x < other.x or \
               (math.isclose(self.x, other.x) and self.y < other.y)
    def __gt__(self, other):
        return not (self == other or self < other)
    def __repr__(self):
        return f'{self.x, self.y}'

    def length(self, base=2):
        return (self.x**base + self.y**base)**(1/base)
    def normalized(self):
        return self / self.length()

    def angle_product(self, other) -> float:
        return self.x * other.y - self.y * other.x
    def dot(self, other) -> float:
        return self.x * other.x + self.y * other.y

class Curvature(Enum):
    TurningAngle = 1
    LengthVariation = 2

class Curve:
    def __init__(self, vertices, curvature=Curvature.TurningAngle):
        to_point = lambda v: v if type(v)==Point else Point(v[0], v[1])
        self.V = [to_point(v) for v in vertices]
        self.n = len(self.V)
        self.E = [self.V[(i+1)%self.n]-self.V[i] for i in range(self.n)]
        self.curvature = curvature
    def __repr__(self):
        return f'Curve(V={self.V}, E={self.E}, n={self.n})'
    def __rmul__(self, scalar):
        return Curve([scalar*v for v in self.V], self.curvature)
    def __add__(self, other):
        if self.n == other.n:
            return Curve([self.V[i]+other.V[i] for i in range(self.n)], self.curvature)
        else:
            raise ValueError
    def __sub__(self, other):
        if self.n == other.n:
            return Curve([self.V[i]-other.V[i] for i in range(self.n)], self.curvature)
        else:
            raise ValueError

    def _update_edges(self):
        self.E = [self.V[(i + 1) % self.n] - self.V[i] for i in range(self.n)]

    def _get_curvature_sign(self, index: int):
        signed = (self.E[index - 1]).angle_product(self.E[index]) 
        return -1 if signed < 0 else 1

    def angle(self, index: int) -> float:
        sign = self._get_curvature_sign(index)
        diff_from = -self.E[index - 1]
        diff_to = self.E[index]
        smallest_part = math.acos(
            diff_from.dot(diff_to) / (diff_from.length() * diff_to.length())
        )
        if sign < 0:
        	return 2 * math.pi - smallest_part
        return smallest_part

    def turning_angle_curvature(self, index: int) -> float:
        return self.angle(index)

    def length_variation_curvature(self, index: int) -> float:
        angle = abs(math.pi - self.angle(index))
        return -2 * math.sin(angle / 2)

    def get_curvature(self, index: int) -> float:
        if self.curvature == Curvature.TurningAngle:
            return self.turning_angle_curvature(index)
        elif self.curvature == Curvature.LengthVariation:
            return self.length_variation_curvature(index)
        else:
            raise ValueError('Unknown curvature type.')

    def N(self, index: int) -> Point:
        '''
        Normalized angle bisector.
        '''
        sign = self._get_curvature_sign(index)

        A = self.E[index - 1].normalized()
        B = -self.E[index].normalized()
        if A == -B:  # In one line
        	return sign * Point(-B.y, B.x)
        return (sign * (A + B)).normalized()
    
    
'''**************************************************************************'''

def basic_flow(c: Curve) -> Curve:
    return 0.2 * Curve(c.E)


def flow(c: Curve) -> Curve:
    return Curve([c.get_curvature(i) * c.N(i) for i in range(c.n)])


def basic_invariant(c: Curve) -> float:
    return c.n


def complete_curvature(c: Curve) -> float:
    curvature = 0
    for i in range(len(c.V)):
        curvature += c.get_curvature(i)
    return curvature


def center_of_mass(c: Curve) -> float:
    mass = Point(0, 0)
    for v in c.V:
        mass += v
    return mass / c.n



def is_regular_polygon(c: Curve) -> bool:
    c._update_edges()
    edge = c.E[0]
    for e in c.E:
        if not math.isclose(e.length(), edge.length()):
            return False

    angle = c.turning_angle_curvature(0)
    for i in range(c.n):
        if not math.isclose(c.turning_angle_curvature(i), angle):
            return False

    return True

'''****************************** Pics **************************************'''
import matplotlib.pyplot as plt


def XY(points: List[Point]):
    X = [v.x for v in points] + [points[0].x]
    Y = [v.y for v in points] + [points[0].y]
    return X, Y


def draw(curve, flow_function, title="Example curve transformation", nsteps=10, step=0.2):
    plt.figure(figsize=(10, 10))
    plt.title(title)
    for i in range(nsteps):        
        plt.plot(*XY(curve.V))
        curve = curve + step*flow_function(curve)
    plt.show()

'''**************************************************************************'''

def is_close(before, after) -> bool:
    if isinstance(before, Point) and isinstance(before, Point):
        return before == after
    elif isinstance(before, (float, int)) and isinstance(before, (float, int)):
        return math.isclose(before, after, abs_tol=EPS)
    else:
        raise ValueError


def check_invariant(c: Curve, flow = basic_flow, invariant = basic_invariant, nsteps=10, step=0.2):
    before = invariant(c)
    print("Инвариант до преобразования: ", before)
    for i in range(nsteps):        
        c = c + step*flow(c)
    after = invariant(c)
    print("Инвариант после преобразования: ", after)
    if is_close(before, after):
      print("Инвариант не изменился")
    elif after > before:
      print("Инвариант увеличился")
    elif after < before:
      print("Инвариант уменьшился")


def check_all_invariants(c: Curve, flow):
    print('Invariant = Complete Curvature')
    check_invariant(c, flow, complete_curvature)

    print('Invariant = Center of Mass')
    check_invariant(c, flow, center_of_mass)

    '''
    print('Invariant = Regular Polygon')
    check_invariant(c, flow, is_regular_polygon)
    '''


'''**************************************************************************'''

class Dataset:
    triangle = Curve([(0, 0),(1, 3),(2, -2),])
    rectangle = Curve([(0, 0), (10, 0), (10, 100), (0, 100)])
    square = Curve([(-1, -1), (1, -1), (1, 1), (-1, 1)])
    star = Curve([(5, 0), (2, 2), (0, 5), (-2, 2), (-5, 0), (-2, -2), (0, -5), (2, -2)])
    convex_polygon = Curve([(-5, -6), (-4, -7), (-2, -8), (1, -9), (5, -10), (8, -8), (6, 1),
    	(3, 3), (1.98, 3.2), (1.06, 3.28), (0, 3.3), (-0.92, 3.2), (-2, 2.8), (-4, 1), (-5, -2)])
    hard_polygon = Curve([(1, -3), (3, -2), (3, -1), (5, -1), (6, 1), (7, 0), (7.6, 2.02), (6, 4),
        (-3.98, 3.76), (3, 3.58), (-5, 3), (0, 2), (5, 3), (6, 3), (6, 2), (-5, 1), (-5, -4),
        (-2, -1), (-1, -4)])

    @staticmethod
    def regular_polygon(n: int, radius: float = 5) -> Curve:
        if n < 1:
            raise ValueError

        angle = 2 * math.pi / n
        vertices = []
        for i in range(n):
            vertices += [(
                radius * math.sin(i * angle),
                radius * math.cos(i * angle)
            )]
        return Curve(vertices)


c = Dataset.convex_polygon

print(c)

'''
print()
print('Basic invariant')
check_invariant(c, basic_flow, basic_invariant)
'''

print()

print('Curvature = Turning Angle')
c.curvature = Curvature.TurningAngle
check_all_invariants(c, flow)
draw(c, flow)

print()

print('Curvature = Length Variation')
c.curvature = Curvature.LengthVariation
check_all_invariants(c, flow)
draw(c, flow)
