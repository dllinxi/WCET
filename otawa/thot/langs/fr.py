
# fr_fr -- i10n for france french
# Copyright (C) 2009  <hugues.casse@laposte.net>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import i18n
import doc

DICT = {
	i18n.ID_CONTENT 		: "Sommaire",
	i18n.ID_INTRODUCTION	: "Introduction",
	i18n.ID_OUTLINE			: "Plan",
	i18n.CAPTION_TABLE		: "Tableau %s: ",
	i18n.CAPTION_FIGURE		: "Figure %s: ",
	i18n.CAPTION_LISTING	: "Listing %s: ",
	i18n.GLYPH_OPEN_DQUOTE	: u"\u00AB",
	i18n.GLYPH_CLOSE_DQUOTE	: u"\u00BB",
	i18n.GLYPH_OPEN_SQUOTE	: "'",
	i18n.GLYPH_CLOSE_SQUOTE	: "'"
}

class FrTranslator(i18n.DictTranslator):
	
	def __init__(self):
		i18n.DictTranslator.__init__(self, DICT)

	def caption(sself, numbering, number):
		if CAPTIONS.has_key(numbering):
			return CAPTIONS[numbering] % number
		else:
			return "%s %s: " % (numbering, number)


def getTranslator(doc, lang):
	return FrTranslator()
