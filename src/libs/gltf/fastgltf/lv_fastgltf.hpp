/**
 * @file lv_fastgltf.hpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include <fastgltf/math.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <functional>
#include <string>

#include "../gltf_data/lv_gltf_data_internal.hpp"

namespace fastgltf
{
/**
 * Computes the transform matrix for a given node
 */
FASTGLTF_EXPORT inline auto getLocalTransformMatrix(const Node& node) {
	return visit_exhaustive(visitor {
		[&](const math::fmat4x4& matrix) {
				return matrix;
		},
		[&](const TRS& trs) {
			/* This may appear backwards, like it is applying the scale last, but it should 
			 * be first.  However, these operations alter the matrix *in place*, they are *not* 
			 * matrix multiplications and they should *not* be applied in the standard 
			 * post-multiplicative order, so this is correct as shown.  This method is used for
			 * performance reasons, since it uses less multiplications and gets the same results.
			 * This function is being added to fastgltf and will be removed from this file soon.
			 */
			return scale(rotate(translate(math::fmat4x4(), trs.translation), trs.rotation), trs.scale);
		}
	}, node.transform);
}

/**
 * Computes the transform matrix for a given node a different way with less total operations
 */
FASTGLTF_EXPORT inline auto getFastLocalTransformMatrix(const Node& node) {
	return visit_exhaustive(visitor {
		[&](const math::fmat4x4& matrix) {
				return matrix;
		},
		[&](const TRS& trs) {
			math::fmat4x4 matrix = math::fmat4x4();
			float sx = trs.scale[0], sy = trs.scale[1], sz = trs.scale[2];
			float qx = trs.rotation[0], qy = trs.rotation[1], qz = trs.rotation[2], qw = trs.rotation[3];
			float x2 = qx + qx, y2 = qy + qy, z2 = qz + qz;
			float xx = qx * x2, xy = qx * y2, xz = qx * z2;
			float yy = qy * y2, yz = qy * z2, zz = qz * z2;
			float wx = qw * x2, wy = qw * y2, wz = qw * z2;
			matrix[0][0] = (1 - (yy + zz)) * sx;
			matrix[0][1] = (xy + wz) * sx;
			matrix[0][2] = (xz - wy) * sx;
			matrix[1][0] = (xy - wz) * sy;
			matrix[1][1] = (1 - (xx + zz)) * sy;
			matrix[1][2] = (yz + wx) * sy;
			matrix[2][0] = (xz + wy) * sz;
			matrix[2][1] = (yz - wx) * sz;
			matrix[2][2] = (1 - (xx + yy)) * sz;
			matrix[3][0] = trs.translation[0];
			matrix[3][1] = trs.translation[1];
			matrix[3][2] = trs.translation[2];
			matrix[0][3] = 0.f;
			matrix[1][3] = 0.f;
			matrix[2][3] = 0.f;
			matrix[3][3] = 1.f;
			return matrix;
		}
	}, node.transform);
}

/**
 * Attempts to remove the scale component of a 4x4 matrix transform.  Will silently fail if 
 * any of the component scales is 0 or near zero (which they should never be).
 */
FASTGLTF_EXPORT inline void removeScale(fastgltf::math::fmat4x4& matrix) {
	auto scale = math::fvec3( length(matrix.col(0)), length(matrix.col(1)), length(matrix.col(2)) );
	if ( ( fabs(scale.x()) > 0.00001f) && (fabs(scale.y()) > 0.00001f) && (fabs(scale.z()) > 0.00001f) ) {
		matrix.col(0) /= scale.x();
		matrix.col(1) /= scale.y();
		matrix.col(2) /= scale.z();
	}
}

FASTGLTF_EXPORT template <typename AssetType, typename Callback>
#if FASTGLTF_HAS_CONCEPTS
    requires std::same_as<std::remove_cvref_t<AssetType>, Asset> &&
    std::is_invocable_v<Callback, fastgltf::Node &, std::string &,
    std::string &, std::size_t, std::size_t>
#endif
void namegen_iterate_scene_nodes(AssetType &&asset,
                                 std::size_t sceneIndex,
                                 Callback callback)
{
    auto & scene = asset.scenes[sceneIndex];

    std::string _id = std::string("");
    std::string _ip = std::string("");
    if(asset.scenes.size() > 1) {
        _id = "scene_" + std::to_string(sceneIndex);
        _ip = std::to_string(sceneIndex);
    }
    auto function = [&](std::size_t nodeIndex, std::string & parentId,
                        std::string & parentIp, std::size_t __child_index,
    auto & self) -> void {
        assert(asset.nodes.size() > nodeIndex);
        auto & node = asset.nodes[nodeIndex];
        std::string _nodeId =
        parentId + std::string("/") + std::string(node.name);
        std::string _nodeIp = parentIp + std::string(".") +
        std::to_string(__child_index);
        std::invoke(callback, node, _nodeId, _nodeIp, nodeIndex,
                    __child_index);
        std::size_t ____child_index = 0;
        for(auto & child : node.children) {
            self(child, _nodeId, _nodeIp, ____child_index, self);
            ____child_index += 1;
        }
    };
    std::size_t child_index = 0;
    for(auto & sceneNode : scene.nodeIndices) {
        function(sceneNode, _id, _ip, child_index, function);
        child_index += 1;
    }
}
FASTGLTF_EXPORT template <typename AssetType, typename Callback>
#if FASTGLTF_HAS_CONCEPTS
    requires std::same_as<std::remove_cvref_t<AssetType>, Asset> &&
    std::is_invocable_v<Callback, fastgltf::Node &, fastgltf::math::fmat4x4 &, fastgltf::math::fmat4x4 &>
#endif
void
findlight_iterate_scene_nodes(AssetType &&asset, std::size_t sceneIndex,
                              math::fmat4x4 * initial, Callback callback)
{
    auto & scene = asset.scenes[sceneIndex];
    auto function = [&](std::size_t nodeIndex,
                        math::fmat4x4 & parentWorldMatrix,
    auto & self) -> void {
        assert(asset.nodes.size() > nodeIndex);
        auto & node = asset.nodes[nodeIndex];
        auto _localMat = getFastLocalTransformMatrix(node);
        std::invoke(callback, node, parentWorldMatrix, _localMat);
        for(auto & child : node.children) {
            math::fmat4x4 _parentWorldTemp =
            parentWorldMatrix * _localMat;
            self(child, _parentWorldTemp, self);
        }
    };
    for(auto & sceneNode : scene.nodeIndices) {
        auto tmat2 = fastgltf::math::fmat4x4(*initial);
        function(sceneNode, tmat2, function);
    }
}
FASTGLTF_EXPORT template <typename Callback>
inline void custom_iterate_scene_nodes(lv_gltf_model_t * model, std::size_t sceneIndex, math::fmat4x4 * initial,
                                       Callback callback)
{
    auto & scene = model->asset.scenes[sceneIndex];

    auto invoke_cb = [&](std::size_t node_index, math::fmat4x4 & parent_world_matrix,
    auto & self) -> void {
        lv_gltf_model_node_t * node = (lv_gltf_model_node_t *)lv_array_at(&model->nodes, node_index);
        fastgltf::Node & fastgltf_node = *node->fastgltf_node;
        LV_ASSERT(node->fastgltf_node == &fastgltf_node);

        auto local_matrix = getFastLocalTransformMatrix(fastgltf_node);
        std::invoke(callback, node, parent_world_matrix, local_matrix);

        uint32_t num_children = fastgltf_node.children.size();
        if(num_children == 0) {
            return;
        }
        math::fmat4x4 new_parent_world_matrix = parent_world_matrix * local_matrix;
        if(num_children == 1)
        {
            self(fastgltf_node.children[0], new_parent_world_matrix,  self);
            return;
        }
        math::fmat4x4 parent_world_matrix_tmp_copy = math::fmat4x4(new_parent_world_matrix);
        for(auto & child : fastgltf_node.children)
        {
            self(child, parent_world_matrix_tmp_copy, self);
        }
    };

    for(size_t i = 0 ; i < scene.nodeIndices.size(); ++i) {
        invoke_cb(scene.nodeIndices[i], *initial, invoke_cb);
    }
}

}


#endif /*LV_USE_GLTF*/
