
/* FABL-ng - FAlling BLocks Next Generation
 * Copyright 2015 (C) Erik Edlund <erik.edlund@32767.se>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define CATCH_CONFIG_RUNNER
#include "fabl.h"

int OS_MAIN(argc, argv)
{
	if (Catch::Session().run(argc, argv)) {
		std::exit(EXIT_FAILURE);
	}
	boost::program_options::options_description opt_desc(pup::default_program_options());
	return pup::run<fabl::application>(argc, argv, opt_desc);
}

