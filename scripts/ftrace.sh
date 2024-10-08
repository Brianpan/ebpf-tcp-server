#!/bin/bash
TRACE_DIR=/sys/kernel/debug/tracing

# clear
echo 0 > $TRACE_DIR/tracing_on
echo > $TRACE_DIR/set_graph_function
echo > $TRACE_DIR/set_ftrace_filter
echo nop > $TRACE_DIR/current_tracer

# setting
echo function_graph > $TRACE_DIR/current_tracer
echo 3 > $TRACE_DIR/max_graph_depth
echo _raw_spin_unlock > $TRACE_DIR/set_graph_function

# execute
echo 1 > $TRACE_DIR/tracing_on
./bench
echo 0 > $TRACE_DIR/tracing_on
