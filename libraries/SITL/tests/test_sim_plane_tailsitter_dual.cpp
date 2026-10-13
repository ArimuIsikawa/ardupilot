#include <AP_gtest.h>
#include <SITL/SIM_Plane.h>
#include <SITL/SITL.h>
#include <utility>

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

class AP_PlaneProbe : public SITL::Plane
{
public:
    AP_PlaneProbe() : Plane("plane-tailsitter-dual")
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

AP_GTEST_MAIN()
