from ConvexHulls import ConvexHull
from HandTarget import HandTarget
from SQP.Q_optimizer import QOptimizer
from SQP.SQPInterface import SQP
from Hand import Hand
import torch
import numpy as np


class Alg2Solver(object):
    def __init__(self, hand_target, sampled_directions, gamma1=0.0001, mu=0.1, scale_closeness=1e6):
        self.hand_target = hand_target
        self.sampled_directions = sampled_directions
        self.gamma1 = gamma1
        self.mu = mu
        self.scale_closeness = scale_closeness

    def qf_solver(self):
        qoptimizer = QOptimizer(self.hand_target, self.sampled_directions, self.gamma1, self.mu, self.scale_closeness)
        Q, F = qoptimizer.optimize()
        return torch.tensor(Q, dtype=torch.double), torch.tensor(F, dtype=torch.double)

    def obj_func(self, params):
        return self.hand_target.alg2_objective(params=params, gamma=self.gamma1)
        # return self.hand_target.hand_target_objective(params=params, gamma=self.gamma)

    def constraints_func(self, params):
        return self.hand_target.friction_cone_constraint(params=params, f=self.F, gamma=self.gamma1, mu=self.mu, scale_closeness=self.scale_closeness)

    def solve(self, x0, niters=100000, plot_interval=30):
        x = x0
        p, _ = self.hand_target.hand.forward(x[:, :hand_target.front])
        self.Q, self.F = self.qf_solver()
        self.old_Q = self.Q
        for i in range(niters):
            sqp_solver = SQP(self.obj_func, self.constraints_func)
            j = 1
            while self.gamma1 > 0.000000001:
                x = sqp_solver.solve(x, plot_interval=plot_interval)
                self.gamma1 *= 0.1
                print(f"Gamma shrinkage{j}: gamma={self.gamma1} x={x[:, :3].detach().numpy()}")
                j += 1
            if x is None:
                print("SQP failed")
                break
            hand_target.reset_parameters(x, True)
            p, _ = hand_target.hand.forward(x[:, :hand_target.front])
            self.old_Q = self.Q
            self.Q, self.F = self.qf_solver()
            print(f'Alg2 Iter{i}: OBJ={self.obj_func(x)}')
            if self.converged():
                print('Alg2 Converged!')
                self.x_optimal = x
                sqp_solver.plot_meshes()
                # self.u_optimal = u
                break
        return x  # , u

    def converged(self, tol=1e-7):
        if np.abs(self.Q - self.old_Q) < tol:
            return True
        else:
            return False


if __name__ == "__main__":
    path = '../hand/BarrettHand/'
    hand = Hand(path, scale=0.01, use_joint_limit=True, use_quat=False, use_eigen=False, use_contacts=False)
    if hand.use_eigen:
        dofs = np.zeros(hand.eg_num)
        params = torch.zeros((1, hand.extrinsic_size + hand.eg_num))
    else:
        dofs = np.zeros(hand.nr_dof())
        params = torch.zeros((1, hand.extrinsic_size + hand.nr_dof()))
    p, t = hand.forward(params)
    # create object
    # target = [ConvexHull(2 * np.random.rand(4, 3) + np.array([-0.2, -0.2, 0.2]))]
    target = [ConvexHull(np.array([[-0.3, -0.3, -0.3],
                                   [-0.3, 0.3, -0.3],
                                   [0.3, -0.3, -0.3],
                                   [0.3, 0.3, -0.3],
                                   [-0.3, 0.3, 0.3],
                                   [0.3, -0.3, 0.3],
                                   [-0.3, -0.3, 0.3],
                                   [0.3, 0.3, 0.3]]) + np.array([0., 0., 0.4]))]
    gamma = 0.0001

    sampled_directions = np.array([[0, 0, 1]])
    hand_target = HandTarget(hand, target)
    # Q, f = QOptimizer(hand_target, sampled_directions, gamma2, mu=0.9).optimize()
    # hand_target = HandTarget(hand, target, alg2=True, Q=Q, F=f, sampled_directions=sampled_directions)
    # hand_target, _ = load_optimizer()
    # sampled_directions = np.array(Directions(res=2, dim=3).dirs)
    alg2_solver = Alg2Solver(hand_target, sampled_directions, gamma1=gamma, mu=0.9)
    x_optimal = alg2_solver.solve(x0=hand_target.params, niters=20, plot_interval=300)