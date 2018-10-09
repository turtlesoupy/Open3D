# Open3D: www.open3d.org
# The MIT License (MIT)
# See license file or visit www.open3d.org for details

# examples/Python/Tutorial/ReconstructionSystem/integrate_scene.py

import numpy as np
import math
import sys
from open3d import *
sys.path.append("../Utility")
from file import *
sys.path.append(".")
from make_fragments import *


def scalable_integrate_rgb_frames(path_dataset, intrinsic, config):
    [color_files, depth_files] = get_rgbd_file_lists(path_dataset)
    n_files = len(color_files)
    n_fragments = int(math.ceil(float(n_files) / \
            config['n_frames_per_fragment']))
    volume = ScalableTSDFVolume(voxel_length = config["tsdf_cubic_size"]/512.0,
            sdf_trunc = 0.04, color_type = TSDFVolumeColorType.RGB8)

    pose_graph_fragment = read_pose_graph(join(
            path_dataset, config["template_refined_posegraph_optimized"]))

    for fragment_id in range(len(pose_graph_fragment.nodes)):
        pose_graph_rgbd = read_pose_graph(join(path_dataset,
                config["template_fragment_posegraph_optimized"] % fragment_id))

        for frame_id in range(len(pose_graph_rgbd.nodes)):
            frame_id_abs = fragment_id * \
                    config['n_frames_per_fragment'] + frame_id
            print("Fragment %03d / %03d :: integrate rgbd frame %d (%d of %d)."
                    % (fragment_id, n_fragments-1, frame_id_abs, frame_id+1,
                    len(pose_graph_rgbd.nodes)))
            rgbd = read_rgbd_image(color_files[frame_id_abs],
                    depth_files[frame_id_abs], False, config)
            pose = np.dot(pose_graph_fragment.nodes[fragment_id].pose,
                    pose_graph_rgbd.nodes[frame_id].pose)
            volume.integrate(rgbd, intrinsic, np.linalg.inv(pose))

    mesh = volume.extract_triangle_mesh()
    mesh.compute_vertex_normals()
    if config["debug_mode"]:
        draw_geometries([mesh])

    mesh_name = join(path_dataset, config["template_global_mesh"])
    write_triangle_mesh(mesh_name, mesh, False, True)


def run(config):
    print("integrate the whole RGBD sequence using estimated camera pose.")
    if config["path_intrinsic"]:
        intrinsic = read_pinhole_camera_intrinsic(config["path_intrinsic"])
    else:
        intrinsic = PinholeCameraIntrinsic(
                PinholeCameraIntrinsicParameters.PrimeSenseDefault)
    scalable_integrate_rgb_frames(config["path_dataset"], intrinsic, config)
