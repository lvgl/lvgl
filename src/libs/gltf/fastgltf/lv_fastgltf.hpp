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
        auto _localMat = getTransformMatrix(node, math::fmat4x4());
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

        auto local_matrix = getTransformMatrix(fastgltf_node, math::fmat4x4());
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
