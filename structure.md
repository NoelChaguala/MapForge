MapForge/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   └── mapforge/
│       ├── core/
│       │   ├── osm_id.hpp
│       │   ├── geo_point.hpp
│       │   ├── node.hpp
│       │   ├── way.hpp
│       │   └── road_metadata.hpp
│       │
│       ├── osm/
│       │   └── osm_parser.hpp
│       │
│       ├── geography/
│       │   └── distance.hpp
│       │
│       ├── graph/
│       │   ├── road_graph.hpp
│       │   └── graph_builder.hpp
│       │
│       ├── routing/
│       │   ├── dijkstra.hpp
│       │   └── astar.hpp
│       │
│       └── spatial/
│           └── spatial_index.hpp
│
├── src/
│   ├── core/
│   ├── osm/
│   ├── geography/
│   ├── graph/
│   ├── routing/
│   └── spatial/
│
├── app/
│   └── main.cpp
│
├── tests/
│   ├── core/
│   ├── osm/
│   ├── geography/
│   ├── graph/
│   ├── routing/
│   └── spatial/
│
├── benchmarks/
│
├── data/
│   └── samples/
│
└── docs/