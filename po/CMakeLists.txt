# for i18n

SET(POFILES
	ar.po
	as.po
	az.po
	bg.po
	bn.po
	ca.po
	cs.po
	da.po
	de.po
	el_GR.po
	en_PH.po
	en.po
	en_US.po
	es_ES.po
	es_US.po
	et.po
	eu.po
	fa.po
	fi.po
	fr_CA.po
	fr.po
	ga.po
	gl.po
	gu.po
	he.po
	hi.po
	hr.po
	hu.po
	hy.po
	id.po
	is.po
	it_IT.po
	ja_JP.po
	ka.po
	kk.po
	km.po
	kn.po
	ko_KR.po
	ky_KG.po
	lo.po
	lt.po
	lv.po
	mk.po
	ml.po
	mn_MN.po
	mr.po
	ms.po
	my_MM.po
	my.po
	my_ZG.po
	nb.po
	ne.po
	nl.po
	or.po
	pa.po
	pl.po
	pt_BR.po
	pt_PT.po
	ro.po
	ru_RU.po
	si.po
	sk.po
	sl.po
	sq.po
	sr.po
	sv.po
	ta.po
	te.po
	tg_TJ.po
	th.po
	tk_TM.po
	tl.po
	tr_TR.po
	uk.po
	ur.po
	uz.po
	vi.po
	zh_CN.po
	zh_HK.po
	zh_TW.po)


FIND_PROGRAM(MSGFMT msgfmt)

FOREACH(POFILE ${POFILES})
GET_FILENAME_COMPONENT(LANG ${POFILE} NAME_WE)

SET(MOFILE ${LANG}.mo)
SET(MOFILES ${MOFILES} ${MOFILE})

	ADD_CUSTOM_COMMAND(
			OUTPUT ${MOFILE}
			COMMAND ${MSGFMT} -o ${MOFILE} ${CMAKE_CURRENT_SOURCE_DIR}/${POFILE}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${POFILE}
			COMMENT "Generating: ${MOFILE}")

	INSTALL(FILES ${MOFILE}
			DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${LANG}/LC_MESSAGES
			RENAME ${PROJECT_NAME}.mo)
ENDFOREACH(POFILE)

ADD_CUSTOM_TARGET(mo ALL DEPENDS ${MOFILES})

