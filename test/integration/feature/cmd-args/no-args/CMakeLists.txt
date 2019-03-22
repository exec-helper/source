set(MODULE "${MODULE}/no-args")

set(FEATURES
        no-args
    )

foreach(FEATURE IN LISTS FEATURES)
    GenerateSimpleRobotTest(${FEATURE} "${MODULE}")
endforeach()
