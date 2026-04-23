local world = {
    objects = {
        field = {
            kind = "location",
            description = "an open field",
            tags = { "field" },
            details = "The field is grim and desolate, location of many battles.",
            contents = "You see",
            gossip = "A lot of corpses over there.",
            capacity = 9999,
            light = 100,
        },
        player = {
            kind = "actor",
            description = "A brave adventurer guinea pig",
            tags = { "yourself" },
            location = "field",
            details = "Because you are a guinea pig, you are small and furious",
            contents = "You have",
            gossip = "What a nice day to be a guinea pig!",
            health = 100,
            capacity = 20,
            impact = -1,
        },
    }
}

return world