/*
 * Freemon: persistent configuration module.
 *
 * Copyright 2013 Emil Brink <emil@obsession.se>.
 * 
 * This file is part of Freemon.
 *
 * Freemon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Freemon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Freemon.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined CONFIG_H_
#define	CONFIG_H_

typedef struct Config	Config;

Config *	config_init(void);

Config *	config_copy(const Config *old);

void		config_delete(Config *cfg);

Config *	config_edit(const Config *cfg, GtkWindow *parent);

#endif		/* CONFIG_H_ */
