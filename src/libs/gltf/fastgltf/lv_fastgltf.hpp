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
#include <functional>
#include <string>

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

FASTGLTF_EXPORT inline void removeScale(fastgltf::math::fmat4x4& matrix) {
	auto scale = math::fvec3( length(matrix.col(0)), length(matrix.col(1)), length(matrix.col(2)) );
	matrix.col(0) /= scale.x();
	matrix.col(1) /= scale.y();
	matrix.col(2) /= scale.z();
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
	auto &scene = asset.scenes[sceneIndex];

	std::string _id = std::string("");
	std::string _ip = std::string("");
	if (asset.scenes.size() > 1) {
		_id = "scene_" + std::to_string(sceneIndex);
		_ip = std::to_string(sceneIndex);
	}
	auto function = [&](std::size_t nodeIndex, std::string &parentId,
			    std::string &parentIp, std::size_t __child_index,
			    auto &self) -> void {
		assert(asset.nodes.size() > nodeIndex);
		auto &node = asset.nodes[nodeIndex];
		std::string _nodeId =
			parentId + std::string("/") + std::string(node.name);
		std::string _nodeIp = parentIp + std::string(".") +
				      std::to_string(__child_index);
		std::invoke(callback, node, _nodeId, _nodeIp, nodeIndex,
			    __child_index);
		std::size_t ____child_index = 0;
		for (auto &child : node.children) {
			self(child, _nodeId, _nodeIp, ____child_index, self);
			____child_index += 1;
		}
	};
	std::size_t child_index = 0;
	for (auto &sceneNode : scene.nodeIndices) {
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
				      math::fmat4x4 *initial, Callback callback)
{
	auto &scene = asset.scenes[sceneIndex];
	auto function = [&](std::size_t nodeIndex,
			    math::fmat4x4 &parentWorldMatrix,
			    auto &self) -> void {
		assert(asset.nodes.size() > nodeIndex);
		auto &node = asset.nodes[nodeIndex];
		auto _localMat = getTransformMatrix(node, math::fmat4x4());
		std::invoke(callback, node, parentWorldMatrix, _localMat);
		for (auto &child : node.children) {
			math::fmat4x4 _parentWorldTemp =
				parentWorldMatrix * _localMat;
			self(child, _parentWorldTemp, self);
		}
	};
	for (auto &sceneNode : scene.nodeIndices) {
		auto tmat2 = fastgltf::math::fmat4x4(*initial);
		function(sceneNode, tmat2, function);
	}
}
FASTGLTF_EXPORT template <typename AssetType, typename Callback>
#if FASTGLTF_HAS_CONCEPTS
    requires std::same_as<std::remove_cvref_t<AssetType>, Asset>
    && std::is_invocable_v<Callback, fastgltf::Node &, fastgltf::math::fmat4x4 &, fastgltf::math::fmat4x4 &>
#endif
inline void custom_iterate_scene_nodes(AssetType&& asset, std::size_t sceneIndex, math::fmat4x4 * initial,
                                     Callback callback)
{
    auto & scene = asset.scenes[sceneIndex];
    auto & nodes = asset.nodes;
    auto function = [&](std::size_t nodeIndex, math::fmat4x4 & parentWorldMatrix, auto & self) -> void {
        //assert(asset.nodes.size() > nodeIndex);
        auto & node = nodes[nodeIndex];
        auto _localMat = getTransformMatrix(node, math::fmat4x4());
        std::invoke(callback, node, parentWorldMatrix, _localMat);
        uint32_t num_children = node.children.size();
        if(num_children > 0) {
            math::fmat4x4 _parentWorldTemp = parentWorldMatrix * _localMat;
            if(num_children > 1) {
                math::fmat4x4 per_child_copy = math::fmat4x4(_parentWorldTemp);
                for(auto & child : node.children) self(child, per_child_copy, self);
            }
            else {
                self(node.children[0], _parentWorldTemp,  self);
            }
        }
    };
    for(auto & sceneNode : scene.nodeIndices) function(sceneNode, *initial, function);
}

FASTGLTF_EXPORT template <typename AssetType, typename Callback>
#if FASTGLTF_HAS_CONCEPTS
    requires std::same_as<std::remove_cvref_t<AssetType>, Asset>
    && std::is_invocable_v<Callback, fastgltf::Node &, uint32_t placement>
#endif
inline void iterate_scene_nodes_get_placement(AssetType&& asset, std::size_t sceneIndex, Callback callback)
{
	uint32_t placement = 0;
    auto & scene = asset.scenes[sceneIndex];
    auto & nodes = asset.nodes;
    auto function = [&](std::size_t nodeIndex, auto & self) -> void {
        auto & node = nodes[nodeIndex];
        std::invoke(callback, node, placement);
		placement++;
		for(auto & child : node.children) self(child, self);
    };
    for(auto & sceneNode : scene.nodeIndices) function(sceneNode, function);
}
}


#endif /*LV_USE_GLTF*/
