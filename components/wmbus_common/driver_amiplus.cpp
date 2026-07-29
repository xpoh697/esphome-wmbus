/*
 Copyright (C) 2019-2025 Fredrik Öhrström (gpl-3.0-or-later)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include"meters_common_implementation.h"

namespace
{
    struct Driver : public virtual MeterCommonImplementation
    {
        Driver(MeterInfo &mi, DriverInfo &di);
    };

    static bool ok = registerDriver([](DriverInfo&di)
    {
        di.setName("amiplus");
        di.setDefaultFields("name,id,total_energy_consumption_kwh,current_power_consumption_kw,total_energy_production_kwh,current_power_production_kw,voltage_at_phase_1_v,voltage_at_phase_2_v,voltage_at_phase_3_v,total_energy_consumption_tariff_1_kwh,total_energy_consumption_tariff_2_kwh,total_energy_consumption_tariff_3_kwh,total_energy_production_tariff_1_kwh,total_energy_production_tariff_2_kwh,total_energy_production_tariff_3_kwh,total_rective_power_l_kvarh,total_rective_power_c_kvarh,current_rective_power_l_var,current_rective_power_c_var,current_at_phase_1_a,current_at_phase_2_a,current_at_phase_3_a,timestamp");

        di.setMeterType(MeterType::ElectricityMeter);
        di.addLinkMode(LinkMode::T1);
        di.addDetection(MANUFACTURER_APA,  0x02,  0x02);
        di.addDetection(MANUFACTURER_DEV,  0x37,  0x02);
        di.addDetection(MANUFACTURER_DEV,  0x02,  0x00);
        di.addDetection(MANUFACTURER_DEV,  0x02,  0x01);
        di.addDetection(MANUFACTURER_APA,  0x02,  0x01);
        
        // Детекция Gama 350 / EGM (0x14ED)
        di.addDetection(0x14ED,  0x02,  0x01);
        di.addDetection(0x14ED,  0x02,  0x02);
        di.addDetection(0x14ED,  0x02,  0x00);

        di.setConstructor([](MeterInfo& mi, DriverInfo& di){ return std::shared_ptr<Meter>(new Driver(mi, di)); });
    });

    Driver::Driver(MeterInfo &mi, DriverInfo &di) : MeterCommonImplementation(mi, di)
    {
        addNumericFieldWithExtractor(
            "total_energy_consumption",
            "The total energy consumption recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            );

        addNumericFieldWithExtractor(
            "current_power_consumption",
            "Current power consumption.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::PowerW)
            );

        addNumericFieldWithExtractor(
            "total_energy_production",
            "The total energy production recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0E833C"))
            );

        addNumericFieldWithExtractor(
            "current_power_production",
            "Current power production.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BAB3C"))
            );

        // Напряжения по фазам (с прямыми ключами Tauron Gama 350)
        addNumericFieldWithExtractor(
            "voltage_at_phase_1",
            "Voltage at phase L1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC9FC01"))
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_2",
            "Voltage at phase L2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC9FC02"))
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_3",
            "Voltage at phase L3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC9FC03"))
            );

        addStringFieldWithExtractor(
            "device_date_time",
            "Device date time.",
            DEFAULT_PRINT_PROPERTIES,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::DateTime)
            );

        // Тарифы потребления (с прямыми ключами Tauron)
        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_1",
            "The total energy consumption recorded by this meter on tariff 1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E1003"))
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_2",
            "The total energy consumption recorded by this meter on tariff 2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E2003"))
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_3",
            "The total energy consumption recorded by this meter on tariff 3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E3003"))
            );

        // Тарифы генерации
        addNumericFieldWithExtractor(
            "total_energy_production_tariff_1",
            "The total energy production recorded by this meter on tariff 1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E10833C"))
            );

        addNumericFieldWithExtractor(
            "total_energy_production_tariff_2",
            "The total energy production recorded by this meter on tariff 2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E20833C"))
            );

        addNumericFieldWithExtractor(
            "total_energy_production_tariff_3",
            "The total energy production recorded by this meter on tariff 3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E30833C"))
            );

        addNumericFieldWithExtractor(
            "max_power_consumption",
            "The maximum demand indicator (maximum 15-min average power consumption recorded this month).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Maximum)
            .set(VIFRange::AnyPowerVIF)
            );

        addNumericFieldWithExtractor(
            "total_rective_power_l",
            "Reactive energy (L).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Reactive_Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0EFB8273"))
        );

        addNumericFieldWithExtractor(
            "total_rective_power_c",
            "Reactive energy (c).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Reactive_Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0EFB82F33C"))
        );

        addNumericFieldWithExtractor(
            "current_rective_power_l",
            "Current ractive power (L).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Reactive_Power,
            VifScaling::None, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFB14")),
            Unit::VAR
        );

        addNumericFieldWithExtractor(
            "current_rective_power_c",
            "Current ractive power (C).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Reactive_Power,
            VifScaling::None, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFB943C")),
            Unit::VAR
        );

        addNumericFieldWithExtractor(
            "current_at_phase_1",
            "Instantaneous current in the L1 phase.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Amperage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFDDAFC01"))
        );

        addNumericFieldWithExtractor(
            "current_at_phase_2",
            "Instantaneous current in the L2 phase.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Amperage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Amperage)
            .set(DifVifKey("0BFDDAFC02"))
        );

        addNumericFieldWithExtractor(
            "current_at_phase_3",
            "Instantaneous current in the L3 phase.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Amperage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFDDAFC03"))
        );
    }
}

KEEP_DRIVER(amiplus);