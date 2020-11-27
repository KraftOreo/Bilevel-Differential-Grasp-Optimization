from ConvexHulls import ConvexHull
from HandObjective import HandObjective
from Optimizer import Optimizer
from Metric import Metric
from Hand import Hand
import numpy as np
import torch

if __name__ == "__main__":
    path = 'hand/BarrettHand/'
    hand = Hand(path, scale=0.01, use_joint_limit=False, use_quat=False, use_eigen=False, use_contacts=False)
    if hand.use_eigen:
        params = torch.rand((1, hand.extrinsic_size + hand.eg_num))
    else:
        params = torch.rand((1, hand.extrinsic_size + hand.nr_dof()))
    p, t = hand.forward(params)

    # create object
    target = [ConvexHull(np.array([[-0.5,-0.5,-0.5],
                                   [-0.5, 0.5,-0.5],
                                   [ 0.5,-0.5,-0.5],
                                   [ 0.5, 0.5,-0.5],
                                   [-0.5, 0.5, 0.5],
                                   [ 0.5,-0.5, 0.5],
                                   [-0.5,-0.5, 0.5],
                                   [ 0.5, 0.5, 0.5]]) + 1.0),
            #   ConvexHull(np.array([[-0.5,-0.5,-0.5],
            #                        [-0.5, 0.5,-0.5],
            #                        [ 0.5,-0.5,-0.5],
            #                        [ 0.5, 0.5,-0.5],
            #                        [-0.5, 0.5, 0.5],
            #                        [ 0.5,-0.5, 0.5],
            #                        [-0.5,-0.5, 0.5],
            #                        [ 0.5, 0.5, 0.5]]) + 2.0)
                                   ]
    metric = Metric(target)
    # metric.setup_distance(hand)
    # metric.draw_samples(.1, 0.5)
    # for i in range(10):
    #     use_numpy = i%2
    #     if use_numpy:
    #         print("numpy: ",metric.compute_metric_numpy(hand))
    #     else:
    #         print("torch: ",metric.compute_metric_torch(hand))
    obj = HandObjective(hand, target, metric)
    gamma = torch.tensor(0.0001, dtype=torch.double)
    alpha = torch.tensor(5, dtype=torch.double)

    def obj_func(param, hand_target):
        return obj.Q_metric_objective(param, gamma, alpha)

    optimizer = Optimizer(obj_func, params=[obj.params, obj], method='Newton')
    optimizer.optimize(niters=10000, plot_interval=10)
    optimizer.plot_meshes()
    optimizer.plot_history().savefig("history.png")
