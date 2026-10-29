# Editable headquarters

Create a **Cheweenie / House Layout** asset at `Assets/Resources/HouseLayout.asset`. Default room names and dimensions are schematic and do not represent the user's actual house.

Room definitions have a plan centre (X/Z), size, story 0/1, floor colour, optional north partition, and optional user photo texture. The builder places labelled room roots and photo panels. Keep user photographs local unless explicitly approved for repository publication.

The default upper loft occupies only the west half, leaving the fort and kitchen open to the camera. A railed ramp connects the southwest lawn to the upstairs landing. Edit both ramp endpoints when changing story height or rooms. Keep the ramp top level with the loft surface, its lower toe below lawn height, the slope gentle, landing open, and at least 1.5m clearance. The builder cuts a channel in the matching loft floor so a buried ramp does not leave a blocking vertical lip. The builder is suitable for an empty-scene prototype; real house dimensions still require user measurements.

The Windows acceptance route walks the ramp up and down through real Rigidbody/controller input. Human acceptance should additionally inspect camera occlusion, edge jumping and custom layout configurations.
