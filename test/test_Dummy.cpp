#include <boost/test/unit_test.hpp>
#include <state_machine/Dummy.hpp>

using namespace state_machine;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    state_machine::DummyClass dummy;
    dummy.welcome();
}
