--[[
	Generic minotaur (also first *.lua NPC ever!)
--]]

c_minotaur_data = c_base_data:extend()
c_minotaur_data.name = "Minotaur"
c_minotaur_data.max_health = 125
c_minotaur_data.max_mana = 1
c_minotaur_data.sprite_sheet_folder = "minotaur"
c_minotaur_data.movement_capability = MOVCAP_NONE

c_minotaur = c_base:extend()
c_minotaur.timer = 1

function c_minotaur:update(delta_time)
	self.timer = self.timer + 1
	if self.timer % 90 == 0 then
		--spawn_creature("minotaur_warrior", Vector(math.random(-4,4),0,3))
	end
end

function c_minotaur:on_spawn()
	print("Spawned minotaur with ID:", self:get_index())
	print(self:get_health())
end

function c_minotaur:on_death()
    print("RIP minotaur with ID:", self:get_index())
end