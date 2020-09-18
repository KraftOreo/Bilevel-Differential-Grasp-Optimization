from ConvexHulls import ConvexHull
from HandTarget import HandTarget
from Q_optimizer import QOptimizer, load_optimizer, obj_func
from MeritFunction import MeritFunction
from SQPInterface import SQP
from Directions import Directions
from Hand import Hand
import torch
import numpy as np


class Alg2Solver(object):
    def __init__(self, hand_target, sampled_directions, gamma=0.001):
        self.hand_target = hand_target
        self.sampled_directions = sampled_directions
        self.gamma = gamma
        self.Q, self.F = self.qf_solver()

    def qf_solver(self):
        qoptimizer = QOptimizer(self.hand_target, self.sampled_directions)
        Q, F = qoptimizer.optimize()
        return torch.tensor(Q, dtype=torch.double), torch.tensor(F, dtype=torch.double)

    def obj_func(self, params):
        return self.hand_target.alg2_objective(params=params, gamma=self.gamma)

    def constraints_func(self, params):
        return self.hand_target.friction_cone_constraint(params=params, f=self.F, gamma=self.gamma)

    def solve(self, x0, niters=100000):
        x = x0
        p, _ = self.hand_target.hand.forward(x[:, :hand_target.front])
        self.Q, self.F = self.qf_solver()
        for i in range(niters):
            sqp_solver = SQP(self.obj_func, self.constraints_func)
            x = sqp_solver.solve(x)
            if x is None:
                print("SQP failed")
                break
            hand_target.reset_parameters(x, True)
            p, _ = hand_target.hand.forward(x[:, :hand_target.front])
            self.Q, self.F = self.qf_solver()
            print(f'Alg2 Iter{i}: OBJ={self.obj_func(x)}')
            if sqp_solver.mf.converged:
                print('Converged!')
                self.x_optimal = x
                # self.u_optimal = u
                break
        return x  # , u

    def kkt(self):
        f_value = self.obj_func(self.x_optimal)
        c_value = self.constraints_func(self.x_optimal)


if __name__ == "__main__":
    path = 'hand/ShadowHand/'
    hand = Hand(path, scale=0.01, use_joint_limit=False, use_quat=False, use_eigen=False, use_contacts=False)
    if hand.use_eigen:
        dofs = np.zeros(hand.eg_num)
        params = torch.zeros((1, hand.extrinsic_size + hand.eg_num))
    else:
        dofs = np.zeros(hand.nr_dof())
        params = torch.zeros((1, hand.extrinsic_size + hand.nr_dof()))
    p, t = hand.forward(params)
    # create object
    target = [ConvexHull(np.random.rand(4, 3) + np.array([0., 0., 1.]))]
    hand_target = HandTarget(hand, target)

    # hand_target, _ = load_optimizer()
    gamma = 0.001
    sampled_directions = np.array(Directions(res=2, dim=3).dirs)
    # sampled_directions = np.array([[1, 1, 1]])
    alg2_solver = Alg2Solver(hand_target, sampled_directions, gamma=gamma)
    x_optimal = alg2_solver.solve(x0=hand_target.params, niters=100)

