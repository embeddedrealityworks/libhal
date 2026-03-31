#include <boost/ut.hpp>
#include <libhal/units.hpp>

int main() {
  using namespace boost::ut::literals;
  using namespace hal::units::literals;
  "freq hz"_test = []() {
    boost::ut::expect(4_Hz == 4.0_Hz);
    boost::ut::expect(3_kHz == 3000_Hz);
    boost::ut::expect(2_MHz == 2000_kHz);
    boost::ut::expect(1_GHz == 1000_MHz);
  };

  "time s"_test = []() {
    boost::ut::expect(1_us == 1000_ns);
    boost::ut::expect(2_ms == 2000_us);
    boost::ut::expect(2_s == 2000_ms);
    boost::ut::expect(3_s == 3000000_us);
  };

  "force N"_test = []() {
    boost::ut::expect(1.0_N == 1000.0_mN);
    boost::ut::expect(2.0_kN == 2000.0_N);
  };

  "current A"_test = []() {
    boost::ut::expect(1.0_mA == 1000.0_uA);
    boost::ut::expect(1.0_A == 1000.0_mA);
  };

  "voltage V"_test = []() {
    boost::ut::expect(1.0_mV == 1000.0_uV);
    boost::ut::expect(2.0_V == 2000.0_mV);
    boost::ut::expect(3.0_kV == 3000.0_V);
  };

  "temperature C"_test = []() {
    boost::ut::expect(1.0_C == 274.15_K);
    boost::ut::expect(-40.0_F == -40.0_C);
  };

  "rpm rev/min"_test = []() {
    boost::ut::expect(10_rpm == 10.0_rpm);
    boost::ut::expect(1000_rpm == 1000.0_rpm);
  };

  "length m"_test = []() {
    boost::ut::expect(1.0_km == 1000.0_m);
    boost::ut::expect(2.0_m == 200.0_cm);
    boost::ut::expect(3.0_m == 3000.0_mm);
    boost::ut::expect(4.0_mm == 4000.0_um);
    boost::ut::expect(1.0_inch == 25.4_mm);
    boost::ut::expect(1.0_yards == 0.9144_m);
    boost::ut::expect(1.0_miles == 1609.344_m);
  };

  "speed m/s"_test = []() {
    boost::ut::expect(100.0_kph == 27.77777777777778_mps);
    boost::ut::expect(10.0_knots == 5.144444444444445_mps);
  };

  "angle radian"_test = []() {
    boost::ut::expect(90.0_deg == 1.5707963267948966_rad);
  };

  "acceleartion m/s^2"_test = []() {
    boost::ut::expect(2.0_G == 19.6133_mps2);
  };

  "mass kg"_test = []() {
    boost::ut::expect(1000.0_g == 1.0_kg);
    boost::ut::expect(1.0_g == 1000.0_mg);
  };
}
