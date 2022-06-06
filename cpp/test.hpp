#pragma once
#define TEST_BEGIN(name) constexpr auto __test__##name = []{
#define TEST_END(name) return 0; }();

