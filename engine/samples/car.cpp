#include <cubos/core/log.hpp>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/null_storage.hpp>

#include <cubos/core/io/window.hpp>

#include <cubos/core/gl/light.hpp>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/data/qb_parser.hpp>

#include <cubos/core/io/input_manager.hpp>
#include <cubos/core/io/sources/double_axis.hpp>
#include <cubos/core/io/sources/single_axis.hpp>
#include <cubos/core/io/sources/button_press.hpp>

#include <cubos/engine/gl/deferred/renderer.hpp>

#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/palette.hpp>

#include <cubos/engine/ecs/position.hpp>
#include <cubos/engine/ecs/rotation.hpp>
#include <cubos/engine/ecs/scale.hpp>
#include <cubos/engine/ecs/local_to_world.hpp>
#include <cubos/engine/ecs/grid.hpp>
#include <cubos/engine/ecs/draw_system.hpp>
#include <cubos/engine/ecs/transform_system.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace cubos;
using namespace cubos::engine;

// Tag for the camera entity.
struct Camera
{
    using Storage = core::ecs::NullStorage<Camera>;
};

// Component for the car entity.
struct Car
{
    using Storage = core::ecs::VecStorage<Car>;

    glm::vec3 vel = {0.0f, 0.0f, 0.0f};
    float angVel = 0.0f;
};

// Stores input data.
struct Input
{
    float deltaTime;
    glm::vec2 lastLook;
    glm::vec2 lookDelta;
    glm::vec3 movement;
    bool cameraEnabled;
};

// System which manages the camera.
class CameraSystem : public core::ecs::IteratingSystem<Camera, ecs::LocalToWorld>
{
public:
    CameraSystem(core::gl::Camera& camera) : camera(camera)
    {
    }

    virtual void process(core::ecs::World&, core::ecs::Entity, Camera&, ecs::LocalToWorld& localToWorld)
    {
        // We want to convert world space to camera space, so we need to invert the matrix.
        this->camera.view = glm::inverse(localToWorld.mat);
    }

private:
    core::gl::Camera& camera;
};

// System which manages the input.
class InputSystem : public core::ecs::System
{
public:
    virtual void init(core::ecs::World& world) override
    {
        Input input;
        input.lastLook = glm::vec2(-1.0f);
        input.lookDelta = glm::vec2(0.0f);
        input.movement = glm::vec3(0.0f);
        input.cameraEnabled = false;
        input.deltaTime = 1.0f / 60.0f;
        world.registerResource<Input>(input);

        auto lookAction = core::io::InputManager::createAction("Look");
        lookAction->addBinding([&](core::io::Context ctx) {
            auto inputResource = world.writeResource<Input>();
            auto& input = inputResource.get();
            auto pos = ctx.getValue<glm::vec2>();

            if (input.lastLook != glm::vec2(-1.0f))
            {
                input.lookDelta = pos - input.lastLook;
            }

            input.lastLook = pos;
        });
        lookAction->addSource(new core::io::DoubleAxis(core::io::MouseAxis::X, core::io::MouseAxis::Y));

        auto forwardAction = core::io::InputManager::createAction("Camera Forward");
        forwardAction->addBinding([&](core::io::Context ctx) {
            auto inputResource = world.writeResource<Input>();
            auto& input = inputResource.get();
            input.movement.z = ctx.getValue<float>();
        });
        forwardAction->addSource(new core::io::SingleAxis(core::io::Key::W, core::io::Key::S));

        auto strafeAction = core::io::InputManager::createAction("Camera Strafe");
        strafeAction->addBinding([&](core::io::Context ctx) {
            auto inputResource = world.writeResource<Input>();
            auto& input = inputResource.get();
            input.movement.x = ctx.getValue<float>();
        });
        strafeAction->addSource(new core::io::SingleAxis(core::io::Key::A, core::io::Key::D));

        auto verticalAction = core::io::InputManager::createAction("Camera Vertical");
        verticalAction->addBinding([&](core::io::Context ctx) {
            auto inputResource = world.writeResource<Input>();
            auto& input = inputResource.get();
            input.movement.y = ctx.getValue<float>();
        });
        verticalAction->addSource(new core::io::SingleAxis(core::io::Key::Q, core::io::Key::E));

        auto enableAction = core::io::InputManager::createAction("Enable Camera");
        enableAction->addBinding([&](core::io::Context ctx) {
            auto inputResource = world.writeResource<Input>();
            auto& input = inputResource.get();
            input.cameraEnabled = !input.cameraEnabled;
        });
        enableAction->addSource(new core::io::ButtonPress(core::io::Key::C));
    }

    virtual void update(core::ecs::World& world) override
    {
        auto inputResource = world.writeResource<Input>();
        inputResource.get().lookDelta = glm::vec2(0.0f);
    }
};

// System which controls the camera.
class CameraControllerSystem : public core::ecs::IteratingSystem<Camera, ecs::Position, ecs::Rotation>
{
private:
    virtual void process(core::ecs::World& world, core::ecs::Entity entity, Camera&, ecs::Position& position,
                         ecs::Rotation& rotation)
    {
        auto inputResource = world.readResource<Input>();
        auto& input = inputResource.get();

        if (input.cameraEnabled)
        {
            // Translate the camera.
            glm::vec3 movement = rotation.quat * input.movement * input.deltaTime * 10.0f;
            position.vec += movement;
        }

        // Rotate the camera.
        auto pitchRot = glm::angleAxis(input.lookDelta.y * -0.002f, glm::vec3(1.0f, 0.0f, 0.0f));
        auto yawRot = glm::angleAxis(input.lookDelta.x * -0.002f, glm::vec3(0.0f, 1.0f, 0.0f));
        auto pitch = glm::angle(rotation.quat * glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rotation.quat = yawRot * rotation.quat;
        if ((pitch > glm::radians(10.0f) || input.lookDelta.y < 0.0f) &&
            (pitch < glm::radians(170.0f) || input.lookDelta.y > 0.0f))
        {
            rotation.quat = rotation.quat * pitchRot;
        }
    }
};

// System which controls the car.
class CarSystem : public core::ecs::IteratingSystem<Car, ecs::Position, ecs::Rotation>
{
private:
    virtual void process(core::ecs::World& world, core::ecs::Entity, Car& car, ecs::Position& position,
                         ecs::Rotation& rotation)
    {
        const float DRAG = 1.0f;
        const float LAT_DRAG = 3.0f;
        const float ANG_DRAG = 1.0f;
        const float TURN_SPEED = 0.2f;
        const float MAX_ANG_VEL = 3.0f;
        const float ACCELERATION = 20.0f;

        auto inputResource = world.readResource<Input>();
        auto& input = inputResource.get();

        // Get direction vectors.
        glm::vec3 forward = rotation.quat * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 strafe = rotation.quat * glm::vec3(1.0f, 0.0f, 0.0f);

        // Get linear velocities.
        float absVel = glm::length(car.vel);
        float forwardVel = glm::dot(car.vel, forward);
        float strafeVel = glm::dot(car.vel, strafe);

        // Update linear velocity.
        if (input.movement.z == 0.0f || input.cameraEnabled)
        {
            if (absVel < 0.02f)
            {
                car.vel = glm::vec3(0.0f);
            }
            else
            {
                car.vel *= glm::max(0.0f, 1.0f - DRAG * input.deltaTime);
            }
        }
        else
        {
            car.vel += forward * ACCELERATION * -input.movement.z * input.deltaTime;
        }
        car.vel -= strafe * strafeVel * LAT_DRAG * input.deltaTime;

        // Update angular velocity.
        car.angVel *= glm::max(0.0f, 1.0f - ANG_DRAG * (1.0f + glm::abs(strafeVel)) * input.deltaTime);
        if (!input.cameraEnabled)
        {
            car.angVel += -input.movement.x * TURN_SPEED * forwardVel * input.deltaTime;
            car.angVel = glm::clamp(car.angVel, -MAX_ANG_VEL, MAX_ANG_VEL);
        }

        // Update position and rotation.
        position.vec += car.vel * input.deltaTime;
        rotation.quat = glm::angleAxis(car.angVel * input.deltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * rotation.quat;
    }
};

void prepareScene(data::AssetManager& assetManager, gl::Renderer& renderer, core::ecs::World& world,
                  core::gl::Palette& palette)
{
    // Generate the floor's grid.
    auto black = palette.add({{0.1f, 0.1f, 0.1f, 1.0f}});
    auto white = palette.add({{0.9f, 0.9f, 0.9f, 1.0f}});
    auto floorGrid = core::gl::Grid({256, 1, 256});
    for (int x = 0; x < 256; ++x)
    {
        for (int z = 0; z < 256; ++z)
        {
            floorGrid.set({x, 0, z}, (x + z) % 2 == 0 ? black : white);
        }
    }
    auto floor = renderer.upload(floorGrid);

    // Spawn the floor.
    world.create(ecs::Grid{floor, {-128.0f, -1.0f, -128.0f}}, ecs::LocalToWorld{}, ecs::Position{}, ecs::Scale{4.0f});

    // Spawn the cars.
    auto car = assetManager.load<data::Grid>("car");
    for (int x = -1; x < 3; ++x)
    {
        for (int y = -1; y < 3; ++y)
        {
            world.create(Car{},
                         ecs::Grid{car->rendererGrid,
                                   {-float(car->grid.getSize().x) / 2.0f, 0.0f, -float(car->grid.getSize().z) / 2.0f}},
                         ecs::LocalToWorld{}, ecs::Position{{36.0f * x, 0.0f, 36.0f * y}}, ecs::Rotation{});
        }
    }

    // Spawn the camera.
    world.create(Camera{}, ecs::LocalToWorld{}, ecs::Position{{0.0f, 40.0f, -70.0f}},
                 ecs::Rotation{glm::quatLookAt(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f})});
}

int main(void)
{
    core::initializeLogger();
    auto window = std::unique_ptr<core::io::Window>(core::io::Window::create());
    window->setMouseState(core::io::MouseState::Locked);

    // Initialize the renderer.
    auto& renderDevice = window->getRenderDevice();
    auto renderer = gl::deferred::Renderer(renderDevice, window->getFramebufferSize());
    auto frame = gl::Frame();

    // Initialize the camera.
    core::gl::Camera camera;
    camera.fovY = 60.0f;
    camera.zNear = 0.1f;
    camera.zFar = 1000.0f;

    // Mount the assets directory.
    core::data::FileSystem::mount("/assets",
                                  std::make_shared<core::data::STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    // Initialize the asset manager.
    auto assetManager = data::AssetManager();
    assetManager.registerType<data::Grid>(&renderer);
    assetManager.registerType<data::Palette>();
    assetManager.importMeta(core::data::FileSystem::find("/assets/"));

    // Initialize the input manager.
    core::io::InputManager::init(window.get());

    // Create the ECS world and initialize the systems.
    core::ecs::World world;
    auto transformSystem = ecs::TransformSystem();
    auto drawSystem = ecs::DrawSystem(frame);
    auto cameraSystem = CameraSystem(camera);
    auto inputSystem = InputSystem();
    auto cameraControllerSystem = CameraControllerSystem();
    auto carSystem = CarSystem();
    transformSystem.init(world);
    drawSystem.init(world);
    cameraSystem.init(world);
    inputSystem.init(world);
    cameraControllerSystem.init(world);
    carSystem.init(world);

    // Get the palette.
    auto paletteAsset = assetManager.load<data::Palette>("palette");
    auto palette = paletteAsset->palette;

    // Prepare the scene.
    prepareScene(assetManager, renderer, world, palette);

    // Set the palette.
    renderer.setPalette(palette);

    // Prepare delta time variables.
    float lastT = 0.0f;
    float deltaT = 0.0f;
    int lastSeconds = 0;

    while (!window->shouldClose())
    {
        // Handle input events.
        window->pollEvents();
        core::io::InputManager::processActions();

        // Calculate the delta time.
        float currentT = window->getTime();
        if (lastT != 0.0f)
        {
            deltaT = currentT - lastT;
            int newSeconds = glm::round(lastT);
            if (newSeconds != lastSeconds)
            {
                core::logDebug("FPS: {}", std::round(1.0f / deltaT));
                lastSeconds = newSeconds;
            }
        }
        lastT = currentT;

        // Clear the frame.
        frame.clear();
        frame.ambient({0.1f, 0.1f, 0.1f});

        // Add a directional light to the frame.
        glm::quat directionalLightRotation = glm::quat(glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0));
        frame.light(core::gl::DirectionalLight(directionalLightRotation, glm::vec3(1), 1.0f));

        // Update the ECS systems.
        cameraControllerSystem.update(world);
        carSystem.update(world);
        transformSystem.update(world);
        cameraSystem.update(world);
        inputSystem.update(world);
        drawSystem.update(world);

        // Render the frame.
        renderer.render(camera, frame);

        window->swapBuffers();
    }

    return 0;
}
