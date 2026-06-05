#include <catch2/catch_test_macros.hpp>
#include <wyrm/serialization.hpp>
#include <wyrm/types.hpp>

#include <iostream>

TEST_CASE("A Description can be serialized/deserialized", "[Serialize/Deserialize Description]") {
    WyrmDescription d1{};
    d1.id = 1;
    d1.parent_id = -1;
    d1.name = "Description1";
    d1.num_markers = 5;

    WyrmDescription d2{};
    d2.id = 2;
    d2.parent_id = -1;
    d2.name = "Description2";
    d2.num_markers = 5;

    WyrmDescription d3{};
    d3.id = 3;
    d3.parent_id = -1;
    d3.name = "Description3";
    d3.num_markers = 5;

    std::unordered_map<int32_t, WyrmDescription> desc = {
        {d1.id, d1},
        {d2.id, d2},
        {d3.id, d3}
    };

    SECTION("Serialize descriptions and Check its size") {
        std::vector<uint8_t> data = SerializeDescriptions(desc);
        REQUIRE( data.size() == 100 );
    }

    SECTION("Deserialize descriptions") {
        std::vector<uint8_t> data = SerializeDescriptions(desc);
        std::unordered_map<int32_t, WyrmDescription> newDesc = DeserializeDescriptions(data);

        // Desc1
        WyrmDescription newd1 = newDesc[d1.id];
        REQUIRE(d1.id == newd1.id);
        REQUIRE(d1.parent_id == newd1.parent_id);
        REQUIRE(d1.name == newd1.name);
        REQUIRE(d1.num_markers == newd1.num_markers);

        // Desc2
        WyrmDescription newd2 = newDesc[d2.id];
        REQUIRE(d2.id == newd2.id);
        REQUIRE(d2.parent_id == newd2.parent_id);
        REQUIRE(d2.name == newd2.name);
        REQUIRE(d2.num_markers == newd2.num_markers);

        // Desc3
        WyrmDescription newd3 = newDesc[d3.id];
        REQUIRE(d3.id == newd3.id);
        REQUIRE(d3.parent_id == newd3.parent_id);
        REQUIRE(d3.name == newd3.name);
        REQUIRE(d3.num_markers == newd3.num_markers);
        
    }
}

TEST_CASE("A Frame of mocap can be serialized/deserialized", "[Serialize/Deserialize Frame]") {
    // Create some rigid bodies for the frame
    WyrmRigidBody b1{};
    b1.id = 1;
    b1.name = "body1";
    b1.x = 0.0f;
    b1.y = 0.0f;
    b1.z = 0.0f;
    b1.qx = 0.0f;
    b1.qy = 0.0f;
    b1.qz = 0.0f;
    b1.qw = 0.0f;
    b1.mean_error = 0.0f;
    b1.tracking_lost = false;
    b1.model_filled = false;

    WyrmRigidBody b2{};
    b2.id = 2;
    b2.name = "body2";
    b2.x = 0.0f;
    b2.y = 0.0f;
    b2.z = 0.0f;
    b2.qx = 0.0f;
    b2.qy = 0.0f;
    b2.qz = 0.0f;
    b2.qw = 0.0f;
    b2.mean_error = 0.0f;
    b2.tracking_lost = false;
    b2.model_filled = false;


    // Create a WyrmFrame to test
    WyrmFrame f{};
    f.frame_id = 1;
    f.timestamp = 0.0f;
    f.precision_timestamp_secs = 0;
    f.precision_timestamp_fractional_secs = 0;
    f.is_recording = true;
    f.model_list_changed = false;
    f.body_count = 2;
    f.bodies = {b1, b2};

    SECTION("Serialize frame f and Check its size") {
        std::vector<uint8_t> data = SerializeFrame(f);
        REQUIRE( data.size() == 132 );
    }

    SECTION("Deserialize frame f") {
        std::vector<uint8_t> data = SerializeFrame(f);
        WyrmFrame newFrame = DeserializeFrame(data);
        REQUIRE(f.frame_id == newFrame.frame_id);
        REQUIRE(f.timestamp == newFrame.timestamp);
        REQUIRE(f.precision_timestamp_secs == newFrame.precision_timestamp_secs);
        REQUIRE(f.precision_timestamp_fractional_secs == newFrame.precision_timestamp_fractional_secs);
        REQUIRE(f.is_recording == newFrame.is_recording);
        REQUIRE(f.model_list_changed == newFrame.model_list_changed);
        REQUIRE(f.body_count == newFrame.body_count);

        // Body 1
        REQUIRE(f.bodies.front().id == newFrame.bodies.front().id);
        REQUIRE(f.bodies.front().name== newFrame.bodies.front().name);
        REQUIRE(f.bodies.front().x == newFrame.bodies.front().x);
        REQUIRE(f.bodies.front().y == newFrame.bodies.front().y);
        REQUIRE(f.bodies.front().z == newFrame.bodies.front().z);
        REQUIRE(f.bodies.front().qx == newFrame.bodies.front().qx);
        REQUIRE(f.bodies.front().qy == newFrame.bodies.front().qy);
        REQUIRE(f.bodies.front().qz == newFrame.bodies.front().qz);
        REQUIRE(f.bodies.front().qw == newFrame.bodies.front().qw);
        REQUIRE(f.bodies.front().mean_error == newFrame.bodies.front().mean_error);
        REQUIRE(f.bodies.front().tracking_lost == newFrame.bodies.front().tracking_lost);
        REQUIRE(f.bodies.front().model_filled == newFrame.bodies.front().model_filled);

        // Body 2
        REQUIRE(f.bodies.back().id == newFrame.bodies.back().id);
        REQUIRE(f.bodies.back().name== newFrame.bodies.back().name);
        REQUIRE(f.bodies.back().x == newFrame.bodies.back().x);
        REQUIRE(f.bodies.back().y == newFrame.bodies.back().y);
        REQUIRE(f.bodies.back().z == newFrame.bodies.back().z);
        REQUIRE(f.bodies.back().qx == newFrame.bodies.back().qx);
        REQUIRE(f.bodies.back().qy == newFrame.bodies.back().qy);
        REQUIRE(f.bodies.back().qz == newFrame.bodies.back().qz);
        REQUIRE(f.bodies.back().qw == newFrame.bodies.back().qw);
        REQUIRE(f.bodies.back().mean_error == newFrame.bodies.back().mean_error);
        REQUIRE(f.bodies.back().tracking_lost == newFrame.bodies.back().tracking_lost);
        REQUIRE(f.bodies.back().model_filled == newFrame.bodies.back().model_filled);
    }
    
}