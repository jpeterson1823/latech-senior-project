#!/bin/bash
docker-compose up -d
source .venv/bin/activate
python3 main.py
