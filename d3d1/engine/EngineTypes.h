//
// Created by Sidorenko Nikita on 4/21/18.
//

#ifndef CPPWRAPPER_ENGTYPES_H
#define CPPWRAPPER_ENGTYPES_H

#include <memory>

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class Material;
typedef std::shared_ptr<Material> MaterialPtr;

class LightObject;
typedef std::shared_ptr<LightObject> LightObjectPtr;

class Projector;
typedef std::shared_ptr<Projector> ProjectorPtr;

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;

class MeshObject;
typedef std::shared_ptr<MeshObject> MeshObjectPtr;

class SkinnedMeshObject;
typedef std::shared_ptr<SkinnedMeshObject> SkinnedMeshObjectPtr;

class ModelBundle;
typedef std::shared_ptr<ModelBundle> ModelBundlePtr;

struct AnimationData;
typedef std::shared_ptr<AnimationData> AnimationDataPtr;

//class MaterialTexutre;
//typedef std::shared_ptr<MaterialTexutre> MaterialTexutrePtr;

class MaterialTextureProjection;
typedef std::shared_ptr<MaterialTextureProjection> MaterialTextureProjectionPtr;

class Terrain;
typedef std::shared_ptr<Terrain> TerrainPtr;

class SpriteSheet;
typedef std::shared_ptr<SpriteSheet> SpriteSheetPtr;

class SwappableFrameBufferObject;
typedef std::shared_ptr<SwappableFrameBufferObject> SwappableFrameBufferObjectPtr;

class RenderTarget;
typedef std::shared_ptr<RenderTarget> RenderTargetPtr;

class ICameraParamsProvider;
typedef std::shared_ptr<ICameraParamsProvider> ICameraParamsProviderPtr;

struct HierarchyData;
typedef std::shared_ptr<HierarchyData> HierarchyDataPtr;

class GameObject;
typedef std::shared_ptr<GameObject> GameObjectPtr;

class PassRenderer;
typedef std::shared_ptr<PassRenderer> PassRendererPtr;

#endif //CPPWRAPPER_ENGTYPES_H

