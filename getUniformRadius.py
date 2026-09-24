import numpy as np
from scipy.spatial.transform import Rotation

def generate_loop_points(L, R, N_pts=800):
    """
    Generates the points (x, y, z) of a square loop with rounded corners in the XY plane.
    L is the length of the straight section.
    R is the corner radius.
    """
    points = []
    n = N_pts // 8
    
    # Upper-right corner
    t = np.linspace(0, np.pi/2, n)
    points.append(np.column_stack((L/2 + R*np.cos(t), L/2 + R*np.sin(t), np.zeros_like(t))))
    # Top straight section
    t = np.linspace(L/2, -L/2, n)
    points.append(np.column_stack((t, (L/2+R)*np.ones_like(t), np.zeros_like(t))))
    # Upper-left corner
    t = np.linspace(np.pi/2, np.pi, n)
    points.append(np.column_stack((-L/2 + R*np.cos(t), L/2 + R*np.sin(t), np.zeros_like(t))))
    # Left straight section
    t = np.linspace(L/2, -L/2, n)
    points.append(np.column_stack((-(L/2+R)*np.ones_like(t), t, np.zeros_like(t))))
    # Lower-left corner
    t = np.linspace(np.pi, 3*np.pi/2, n)
    points.append(np.column_stack((-L/2 + R*np.cos(t), -L/2 + R*np.sin(t), np.zeros_like(t))))
    # Bottom straight section
    t = np.linspace(-L/2, L/2, n)
    points.append(np.column_stack((t, -(L/2+R)*np.ones_like(t), np.zeros_like(t))))
    # Lower-right corner
    t = np.linspace(3*np.pi/2, 2*np.pi, n)
    points.append(np.column_stack((L/2 + R*np.cos(t), -L/2 + R*np.sin(t), np.zeros_like(t))))
    # Right straight section
    t = np.linspace(-L/2, L/2, n)
    points.append(np.column_stack(((L/2+R)*np.ones_like(t), t, np.zeros_like(t))))
    
    return np.vstack(points)

def calc_B(pts, eval_pt):
    """Applies the Biot-Savart law to a wire composed of small straight segments."""
    dl = pts[1:] - pts[:-1]
    midpts = (pts[:-1] + pts[1:]) / 2.0
    r_vec = eval_pt - midpts
    r_mag = np.linalg.norm(r_vec, axis=1)
    
    # Avoid division by zero on the wire itself
    r_mag[r_mag == 0] = 1e-10 
    
    cross = np.cross(dl, r_vec)
    B = np.sum(cross / (r_mag**3)[:, None], axis=0)
    return B

def B_helmholtz(L, R, D, eval_pt, axis='Z'):
    """Evaluates the sum of two Helmholtz-configured loops for a given axis."""
    pts = generate_loop_points(L, R)
    pts = np.vstack([pts, pts[0]]) # Cerramos la espira
    
    pts1, pts2 = pts.copy(), pts.copy()
    pts1[:,2] = D/2
    pts2[:,2] = -D/2
    
    # Rotation depending on the axis being evaluated
    if axis == 'X':
        rot = Rotation.from_euler('y', 90, degrees=True)
        pts1, pts2 = rot.apply(pts1), rot.apply(pts2)
    elif axis == 'Y':
        rot = Rotation.from_euler('x', 90, degrees=True)
        pts1, pts2 = rot.apply(pts1), rot.apply(pts2)
        
    return calc_B(pts1, eval_pt) + calc_B(pts2, eval_pt)

def find_radius(L, R, D, axis, tol):
    """Finds the spherical radius using binary search and Fibonacci-like sampling."""
    # Uniform distribution of points on a sphere
    samples = 50
    phi = np.pi * (3. - np.sqrt(5.))
    y = 1 - (np.arange(samples) / float(samples - 1)) * 2
    radius = np.sqrt(1 - y * y)
    theta = phi * np.arange(samples)
    
    x = np.cos(theta) * radius
    z = np.sin(theta) * radius
    dirs = np.column_stack((x, y, z))
    
    # Field at the center
    B0 = np.linalg.norm(B_helmholtz(L, R, D, np.array([0,0,0]), axis))
    
    # Binary search for the radius
    low, high = 0.0, min(L, D) / 2 * 0.95
    best_r = 0.0
    
    for _ in range(15): # 15 iterations ensure convergence
        mid = (low + high) / 2
        max_dev = 0
        for d in dirs:
            pt = mid * d
            B = np.linalg.norm(B_helmholtz(L, R, D, pt, axis))
            dev = abs(B - B0) / B0
            if dev > max_dev: 
                max_dev = dev
                
        if max_dev > tol:
            high = mid
        else:
            low = mid
            best_r = mid
            
    return best_r

# ================================
# MAIN DATA EXECUTION
# ================================
R_corners    = 4.5
axes_data = [
    ('Z', 120, 63.2),
    ('Y', 138, 73.1),
    ('X', 129, 68.6)
]
tolerances = [0.01, 0.02, 0.05]

for name, L, D in axes_data:
    print(f"--- Evaluating Axis {name} ---")
    for t in tolerances:
        r = find_radius(L, R_corners, D, name, t)
        print(f"Tolerance {t*100}% : Maximum volume radius = {r:.2f} cm")
