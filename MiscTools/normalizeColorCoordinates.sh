#!/bin/bash

echo "enter R component: "
read RComponent

echo "enter G component: "
read GComponent

echo "enter B component: "
read BComponent

let maxValue=255

echo "R: $RComponent/$maxValue" | calc -dp
echo "G: $GComponent/$maxValue" | calc -dp
echo "B: $BComponent/$maxValue" | calc -dp