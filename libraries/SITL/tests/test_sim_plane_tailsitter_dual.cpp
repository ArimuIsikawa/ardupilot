#include <AP_gtest.h>
#include <SITL/SIM_Plane.h>
#include <SITL/SITL.h>
#include <utility>

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

class AP_PlaneProbe : public SITL::Plane
{
public:
    explicit AP_PlaneProbe(const char *frame_name = "plane-tailsitter-dual") : Plane(frame_name)
    {
        if (AP::sitl() == nullptr) {
            static SITL::SIM simulation;
        }
        sitl = AP::sitl();
        sitl->batt_voltage.set(12.6f);
        sitl->servo.servo_speed.set(0);
        frame_time_us = 2500;
        reverse_elevator_rudder = false;
        elevons = vtail = dspoilers = redundant = reverse_thrust = false;
        ice_engine = aerobatic = have_launcher = false;
        gyro.zero();
        velocity_air_bf.zero();
        airspeed = 0;
        motor_mask = 0;
        for (float &motor_speed : rpm) {
            motor_speed = 0;
        }
    }

    Vector3f forces(const sitl_input& input)
    {
        Vector3f angular_acceleration;
        calculate_forces(input, angular_acceleration);
        return angular_acceleration;
    }

    void set_air_velocity(const Vector3f& velocity)
    {
        velocity_air_bf = velocity;
        airspeed = velocity.length();
    }

    bool on_ground() const override
    {
        return false;
    }

    uint32_t motors() const
    {
        return motor_mask;
    }
    float motor_rpm(uint8_t channel) const
    {
        return rpm[channel];
    }
    float current() const
    {
        return battery_current;
    }
};

static sitl_input neutral_input()
{
    sitl_input input{};
    input.servos[0] = input.servos[1] = 1500;
    input.servos[4] = input.servos[5] = 1500;
    return input;
}

TEST(PlaneTailsitterDual, TwoFixedMotors)
{
    AP_PlaneProbe plane;
    auto input = neutral_input();
    const Vector3f angular_acceleration = plane.forces(input);
    EXPECT_EQ(plane.motors(), (1U << 4) | (1U << 5));
    EXPECT_FLOAT_EQ(plane.motor_rpm(4), 3500);
    EXPECT_FLOAT_EQ(plane.motor_rpm(5), 3500);
    EXPECT_NEAR(angular_acceleration.z, 0, 1.0e-6);
}

TEST(PlaneTailsitterDual, DifferentialThrustYaw)
{
    auto input = neutral_input();
    input.servos[4] = 1800;
    input.servos[5] = 1200;
    AP_PlaneProbe left;
    const float left_yaw = left.forces(input).z;
    std::swap(input.servos[4], input.servos[5]);
    AP_PlaneProbe right;
    const float right_yaw = right.forces(input).z;
    EXPECT_GT(left_yaw, 0);
    EXPECT_LT(right_yaw, 0);
    EXPECT_NEAR(left_yaw, -right_yaw, 1.0e-6);
}

TEST(PlaneTailsitterDual, LateralFlowIgnoresPropellerWash)
{
    AP_PlaneProbe low_thrust;
    low_thrust.set_air_velocity(Vector3f{0, 5, 0});
    auto input = neutral_input();
    const float yaw = low_thrust.forces(input).z;
    input.servos[4] = input.servos[5] = 1900;
    AP_PlaneProbe high_thrust;
    high_thrust.set_air_velocity(Vector3f{0, 5, 0});
    EXPECT_GT(yaw, 0);
    EXPECT_NEAR(high_thrust.forces(input).z, yaw, 1.0e-6);
}

TEST(PlaneTailsitterDual, NoVirtualThrottleOrRudder)
{
    auto input = neutral_input();
    AP_PlaneProbe baseline;
    const Vector3f baseline_acceleration = baseline.forces(input);
    input.servos[2] = 2000;
    input.servos[3] = 2000;
    AP_PlaneProbe changed;
    const Vector3f changed_acceleration = changed.forces(input);
    EXPECT_NEAR((changed_acceleration - baseline_acceleration).length(), 0, 1.0e-6);
    EXPECT_EQ(changed.motors(), baseline.motors());
    EXPECT_FLOAT_EQ(changed.motor_rpm(4), baseline.motor_rpm(4));
    EXPECT_FLOAT_EQ(changed.motor_rpm(5), baseline.motor_rpm(5));
}

TEST(PlaneTailsitterDual, ElevonControl)
{
    auto input = neutral_input();
    AP_PlaneProbe neutral;
    const Vector3f baseline = neutral.forces(input);
    input.servos[0] = 1200;
    input.servos[1] = 1800;
    AP_PlaneProbe roll;
    EXPECT_GT(roll.forces(input).x, baseline.x);
    input.servos[0] = input.servos[1] = 1800;
    AP_PlaneProbe pitch;
    EXPECT_GT(pitch.forces(input).y, baseline.y);
}

static sitl_input real_layout_input(const sitl_input &abstract_input)
{
    sitl_input input{};
    input.servos[0] = abstract_input.servos[4];
    input.servos[1] = abstract_input.servos[5];
    input.servos[4] = input.servos[5] = 3000 - abstract_input.servos[0];
    input.servos[6] = input.servos[7] = abstract_input.servos[1];
    return input;
}

TEST(PlaneTailsitterDual, RealLayoutTwoFixedMotors)
{
    AP_PlaneProbe plane("plane-tailsitter-dual-real");
    const Vector3f angular_acceleration = plane.forces(real_layout_input(neutral_input()));
    EXPECT_EQ(plane.motors(), (1U << 0) | (1U << 1));
    EXPECT_FLOAT_EQ(plane.motor_rpm(0), 3500);
    EXPECT_FLOAT_EQ(plane.motor_rpm(1), 3500);
    for (uint8_t channel = 2; channel < 8; channel++) {
        EXPECT_FLOAT_EQ(plane.motor_rpm(channel), 0);
    }
    EXPECT_NEAR(angular_acceleration.z, 0, 1.0e-6);
}

TEST(PlaneTailsitterDual, RealLayoutMatchesAbstractForces)
{
    const struct {
        uint16_t left_elevon;
        uint16_t right_elevon;
        uint16_t left_motor;
        uint16_t right_motor;
    } controls[] = {
        {1500, 1500, 1500, 1500},
        {1200, 1800, 1500, 1500},
        {1800, 1800, 1500, 1500},
        {1500, 1500, 1800, 1200},
        {1500, 1500, 1200, 1800},
        {1600, 1700, 1900, 1600},
        {1300, 1400, 1000, 2000},
    };
    const Vector3f velocities[] = {{0, 0, 0}, {27, 0, 0}, {0, 5, 0}, {20, -3, 2}};
    for (const auto &control : controls) {
        auto input = neutral_input();
        input.servos[0] = control.left_elevon;
        input.servos[1] = control.right_elevon;
        input.servos[4] = control.left_motor;
        input.servos[5] = control.right_motor;
        for (const Vector3f &velocity : velocities) {
            AP_PlaneProbe abstract_plane;
            AP_PlaneProbe real_plane("plane-tailsitter-dual-real");
            abstract_plane.set_air_velocity(velocity);
            real_plane.set_air_velocity(velocity);
            const Vector3f abstract_acceleration = abstract_plane.forces(input);
            const Vector3f real_acceleration = real_plane.forces(real_layout_input(input));
            EXPECT_NEAR((real_acceleration - abstract_acceleration).length(), 0, 1.0e-5);
            EXPECT_FLOAT_EQ(real_plane.current(), abstract_plane.current());
            EXPECT_FLOAT_EQ(real_plane.motor_rpm(0), abstract_plane.motor_rpm(4));
            EXPECT_FLOAT_EQ(real_plane.motor_rpm(1), abstract_plane.motor_rpm(5));
        }
    }
}

TEST(PlaneTailsitterDual, RealLayoutAveragesEachElevonPair)
{
    for (uint8_t channel = 4; channel < 8; channel++) {
        auto input = real_layout_input(neutral_input());
        auto averaged_input = input;
        input.servos[channel] = 1700;
        const uint8_t pair_start = channel < 6 ? 4 : 6;
        averaged_input.servos[pair_start] = averaged_input.servos[pair_start + 1] = 1600;
        AP_PlaneProbe plane("plane-tailsitter-dual-real");
        AP_PlaneProbe averaged_plane("plane-tailsitter-dual-real");
        const Vector3f acceleration = plane.forces(input);
        const Vector3f averaged_acceleration = averaged_plane.forces(averaged_input);
        EXPECT_NEAR((acceleration - averaged_acceleration).length(), 0, 1.0e-5);
        EXPECT_FLOAT_EQ(plane.motor_rpm(0), 3500);
        EXPECT_FLOAT_EQ(plane.motor_rpm(1), 3500);
    }
}

TEST(PlaneTailsitterDual, RealLayoutIgnoresPassThroughOutputs)
{
    auto input = real_layout_input(neutral_input());
    AP_PlaneProbe baseline("plane-tailsitter-dual-real");
    const Vector3f baseline_acceleration = baseline.forces(input);
    input.servos[2] = 2000;
    input.servos[3] = 1000;
    AP_PlaneProbe changed("plane-tailsitter-dual-real");
    const Vector3f changed_acceleration = changed.forces(input);
    EXPECT_NEAR((changed_acceleration - baseline_acceleration).length(), 0, 1.0e-6);
    EXPECT_EQ(changed.motors(), baseline.motors());
    EXPECT_FLOAT_EQ(changed.motor_rpm(0), baseline.motor_rpm(0));
    EXPECT_FLOAT_EQ(changed.motor_rpm(1), baseline.motor_rpm(1));
    EXPECT_FLOAT_EQ(changed.current(), baseline.current());
}

AP_GTEST_MAIN()
