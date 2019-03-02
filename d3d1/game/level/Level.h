//
// Created by Sidorenko Nikita on 2018-12-18.
//

#ifndef CPPWRAPPER_LEVEL_H
#define CPPWRAPPER_LEVEL_H

#include "Engine.h"
#include "EngineTypes.h"
#include <string>
#include <unordered_map>

class Level {
public:
  Level(ScenePtr &scene, SpriteSheetPtr &decalsSpritesheet, TexturePtr &decalsTexture);
  void load(std::string fileName);

private:
  GameObjectPtr _loadHierarchy(HierarchyDataPtr hierarchy, const GameObjectPtr parent);

  GameObjectPtr _levelRoot;
  ScenePtr _scene;
  SpriteSheetPtr _decalsSpritesheet;
  ModelBundlePtr _architecture;
  ModelBundlePtr _props;
  ModelBundlePtr _level;
  TexturePtr _decalsTexture;
  TexturePtr _flareTexture;

  std::unordered_map<std::string, TexturePtr> _textures;

private:
  std::string _getDecalName(const std::string &objectName);
  void _setSphereBounds(GameObjectPtr &object);
  GameObjectPtr _createLight(HierarchyDataPtr &child);
  GameObjectPtr _createProjector(HierarchyDataPtr &child);
  MeshObjectPtr _createMeshObject(ModelBundlePtr &bundle, HierarchyDataPtr &referenceNode, GameObjectPtr &parent);
  void _assignLayer(GameObjectPtr &parent);
};


#endif //CPPWRAPPER_LEVEL_H
