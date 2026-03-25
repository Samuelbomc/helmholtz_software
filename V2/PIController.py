import numpy as np


class PIController:
    """A simple Proportional-Integral (PI) controller."""
    def __init__(self, Kp: float, Ki: float):
        self.Kp = Kp
        self.Ki = Ki
        self.integral_error = 0.0
        self.last_target = None

    def calculate(self, target_value: float, current_value: float) -> float:
        """Calculates the required adjustment based on the error."""
        if self.last_target is not None and not np.isclose(target_value, self.last_target):
            self.reset()
        self.last_target = target_value

        error = target_value - current_value
        self.integral_error += error
        self.integral_error = max(min(self.integral_error, 20.0), -20.0)

        return (self.Kp * error) + (self.Ki * self.integral_error)

    def reset(self):
        """Resets the integral error."""
        self.integral_error = 0.0