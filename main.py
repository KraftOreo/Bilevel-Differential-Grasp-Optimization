from ConvexHulls import ConvexHull
from HandTarget import HandTarget
from Optimizer import Optimizer
from Hand import Hand
import torch, trimesh
import numpy as np

data_type = torch.double

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
    target = [ConvexHull(np.random.rand(4, 3) + 1.)]
    hand_target = HandTarget(hand, target)
    gamma = torch.tensor(0.001, dtype=data_type)


    def obj_func(param, hand_target):
        return hand_target.hand_target_objective(param, gamma)


    optimizer = Optimizer(obj_func, params=[hand_target.params, hand_target], method='Newton')
    optimizer.optimize(niters=100000, plot_interval=20)
    optimizer.plot_history().savefig("history.png")
    optimizer.plot_meshes()
