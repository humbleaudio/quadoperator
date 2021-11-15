// Copyright 2017 Humble Audio LLC

#include "quadop/state.h"

#include <conf/arch_conf.h>

namespace quadop {

quadop::OperatorBank State::bank;
quadop::Algo State::algo;
quadop::Calibration State::cal;
quadop::CvTest State::cvTest;
quadop::Dac State::dac;
quadop::ExternalAdc State::extAdc;
quadop::InternalAdc State::intAdc;
quadop::PwmTimer State::leds;
quadop::DigitalPins State::pins;
quadop::Parameters State::params;
quadop::Scheduler State::scheduler;
quadop::Settings State::settings;

volatile uint32_t State::sampleCount;
volatile bool State::algoAttached;
volatile pfloat_t State::algoLeftGain;
volatile pfloat_t State::algoRightGain;
volatile OperatorBankParameters State::bankParams;

}  // namespace quadop
