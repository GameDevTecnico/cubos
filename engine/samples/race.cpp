#include <cubos/core/log.hpp>

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/null_storage.hpp>
#include <cubos/core/ecs/system.hpp>

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
    using Storage = core::ecs::VecStorage<Camera>;

    core::ecs::Entity car;
};

// Component for the car entity.
struct Car
{
    using Storage = core::ecs::VecStorage<Car>;

    glm::vec3 vel = {0.0f, 0.0f, 0.0f};
    float angVel = 0.0f;
};

// Resource which stores input data.
struct Input
{
    float deltaTime;
    glm::vec3 movement;
};

// System which sets the position of the camera resource from the camera entity.
void cameraSystem(core::gl::Camera& cameraRsc, core::ecs::Query<const Camera&, const ecs::LocalToWorld&> query)
{
    for (auto [entity, camera, localToWorld] : query)
    {
        cameraRsc.view = glm::inverse(localToWorld.mat);
    }
}

// Function which setups input callbacks and sets up the input resource.
void setupInput(core::ecs::World& world)
{
    Input input;
    input.movement = glm::vec3(0.0f);
    input.deltaTime = 1.0f / 60.0f;
    world.registerResource<Input>(input);

    auto forwardAction = core::io::InputManager::createAction("Camera Forward");
    forwardAction->addBinding([&](core::io::Context ctx) {
        auto inputResource = world.write<Input>();
        auto& input = inputResource.get();
        input.movement.z = ctx.getValue<float>();
    });
    forwardAction->addSource(new core::io::SingleAxis(core::io::Key::W, core::io::Key::S));

    auto strafeAction = core::io::InputManager::createAction("Camera Strafe");
    strafeAction->addBinding([&](core::io::Context ctx) {
        auto inputResource = world.write<Input>();
        auto& input = inputResource.get();
        input.movement.x = ctx.getValue<float>();
    });
    strafeAction->addSource(new core::io::SingleAxis(core::io::Key::A, core::io::Key::D));

    auto verticalAction = core::io::InputManager::createAction("Camera Vertical");
    verticalAction->addBinding([&](core::io::Context ctx) {
        auto inputResource = world.write<Input>();
        auto& input = inputResource.get();
        input.movement.y = ctx.getValue<float>();
    });
    verticalAction->addSource(new core::io::SingleAxis(core::io::Key::Q, core::io::Key::E));
}

// System which updates the transform of the camera.
void cameraControllerSystem(core::ecs::Query<const Camera&, ecs::Position&, ecs::Rotation&> cameraQuery,
                            core::ecs::Query<const Car&, const ecs::Position&, const ecs::Rotation&> carQuery)
{
    for (auto [entity, camera, position, rotation] : cameraQuery)
    {
        // Get the car position and rotation.
        auto [car, carPosition, carRotation] = carQuery[camera.car].value();

        glm::vec3 forward = glm::normalize(car.vel);
        if (glm::length2(car.vel) < 0.01f)
        {
            forward = carRotation.quat * glm::vec3(0.0f, 0.0f, 1.0f);
        }
        forward.y -= 0.2f;
        forward = glm::normalize(forward);
        position.vec = carPosition.vec - forward * 70.0f + glm::vec3(0.0f, 30.0f, 0.0f);
        rotation.quat = glm::quatLookAt(forward, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

// System which controls and updates the car.
void carSystem(const Input& input, core::ecs::Query<Car&, ecs::Position&, ecs::Rotation&> query)
{
    const float DRAG = 1.0f;
    const float LAT_DRAG = 3.0f;
    const float ANG_DRAG = 1.0f;
    const float TURN_SPEED = 0.2f;
    const float MAX_ANG_VEL = 3.0f;
    const float ACCELERATION = 20.0f;

    for (auto [entity, car, position, rotation] : query)
    {
        // Get direction vectors.
        glm::vec3 forward = rotation.quat * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 strafe = rotation.quat * glm::vec3(1.0f, 0.0f, 0.0f);

        // Get linear velocities.
        float absVel = glm::length(car.vel);
        float forwardVel = glm::dot(car.vel, forward);
        float strafeVel = glm::dot(car.vel, strafe);

        // Update linear velocity.
        if (input.movement.z == 0.0f)
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
        car.angVel += -input.movement.x * TURN_SPEED * forwardVel * input.deltaTime;
        car.angVel = glm::clamp(car.angVel, -MAX_ANG_VEL, MAX_ANG_VEL);

        // Update position and rotation.
        position.vec += car.vel * input.deltaTime;
        rotation.quat = glm::angleAxis(car.angVel * input.deltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * rotation.quat;
    }
}

void prepareScene(data::AssetManager& assetManager, gl::Renderer& renderer, core::ecs::World& world,
                  core::gl::Palette& palette)
{
    // Generate the floor's grid.
    uint16_t colors[] = {
        palette.add({{0.1f, 0.1f, 0.1f, 1.0f}}), palette.add({{0.9f, 0.9f, 0.9f, 1.0f}}),
        palette.add({{0.9f, 0.1f, 0.1f, 1.0f}}), palette.add({{0.1f, 0.9f, 0.1f, 1.0f}}),
        palette.add({{0.1f, 0.1f, 0.9f, 1.0f}}), palette.add({{0.9f, 0.9f, 0.1f, 1.0f}}),
        palette.add({{0.1f, 0.9f, 0.9f, 1.0f}}), palette.add({{0.9f, 0.1f, 0.9f, 1.0f}}),
    };
    auto floorGrid = core::gl::Grid({256, 1, 256});
    for (int x = 0; x < 256; ++x)
    {
        for (int z = 0; z < 256; ++z)
        {
            floorGrid.set({x, 0, z}, colors[rand() % 8]);
        }
    }
    auto floor = renderer.upload(floorGrid);

    // Spawn the floor.
    world.create(ecs::Grid{floor, {-128.0f, -1.0f, -128.0f}}, ecs::LocalToWorld{}, ecs::Position{}, ecs::Scale{4.0f});

    // Spawn the car.
    auto carAsset = assetManager.load<data::Grid>("car");
    auto car = world.create(
        Car{},
        ecs::Grid{carAsset->rendererGrid,
                  {-float(carAsset->grid.getSize().x) / 2.0f, 0.0f, -float(carAsset->grid.getSize().z) / 2.0f}},
        ecs::LocalToWorld{}, ecs::Position{}, ecs::Rotation{});

    // Spawn the camera.
    world.create(Camera{car}, ecs::LocalToWorld{}, ecs::Position{{0.0f, 40.0f, -70.0f}},
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

    // Create the ECS world and register the necessary components.
    core::ecs::World world;
    world.registerComponent<ecs::Grid>();
    world.registerComponent<ecs::LocalToWorld>();
    world.registerComponent<ecs::Position>();
    world.registerComponent<ecs::Rotation>();
    world.registerComponent<ecs::Scale>();
    world.registerComponent<Camera>();
    world.registerComponent<Car>();

    // Add the frame and camera resources.
    world.registerResource<gl::Frame>();
    {
        core::gl::Camera camera;
        camera.fovY = 60.0f;
        camera.zNear = 0.1f;
        camera.zFar = 1000.0f;
        world.registerResource<core::gl::Camera>(camera);
    }

    // Prepare the input resource.
    setupInput(world);

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

        // Clear the frame and add a light to it.
        {
            auto frame = world.write<gl::Frame>();
            frame.get().clear();
            frame.get().ambient({0.1f, 0.1f, 0.1f});

            // Add a directional light to the frame.
            glm::quat directionalLightRotation = glm::quat(glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0));
            frame.get().light(core::gl::DirectionalLight(directionalLightRotation, glm::vec3(1), 1.0f));
        }

        // Update the ECS systems.

        auto cmds = core::ecs::Commands(world);
        core::ecs::SystemWrapper(cameraControllerSystem).call(world, cmds);
        core::ecs::SystemWrapper(carSystem).call(world, cmds);
        core::ecs::SystemWrapper(ecs::transformSystem).call(world, cmds);
        core::ecs::SystemWrapper(cameraSystem).call(world, cmds);
        core::ecs::SystemWrapper(ecs::drawSystem).call(world, cmds);

        // Render the frame.
        {
            auto camera = world.read<core::gl::Camera>();
            auto frame = world.read<gl::Frame>();
            renderer.render(camera.get(), frame.get());
        }

        window->swapBuffers();
    }

    return 0;
}
