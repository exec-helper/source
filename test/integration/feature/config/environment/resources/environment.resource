*** Keywords ***
the call is executed within the given environment
    [Arguments]     ${command}      ${env}
    environment_contains   ${command}      ${env}
