import pytest
from pytest_bdd import scenarios

from scenarios_run import *

scenarios(
    "../feature/execution-order/execution-order.feature",
    example_converters=dict(command=str, nb_of_times=int),
)
