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
            details = "You have awakened in a horrible place where big battle between animals took place",
            contents = "You have",
            gossip = "What a nice day to be a guinea pig!",
            health = 100,
            capacity = 20,
            impact = -1,
        },
        dogCorpse = {
			kind = "actor",
            description = "A valiant dog soldier slain on a field",
            tags = { "dog", "dog corspe" },
            location = "field",
            --details = "",
            contents = "In dog pockets he had ",
            health = 0,
            capacity = 20,
		},
		--catCorpse = {
			--kind = "actor",
            --description = "An archer from cat sultanate",
		--},
    }
}

return world
