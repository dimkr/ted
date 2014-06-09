/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docStyleSheet.h"

/************************************************************************/
/*									*/
/*  Style administration.						*/
/*									*/
/************************************************************************/

void docInitDocumentStyle(	DocumentStyle *	ds )
    {
    ds->dsStyleNumber= -1;
    ds->dsBasedOn= -1;
    ds->dsAdditive= 0;
    ds->dsNext= -1;
    ds->dsLink= -1;
    ds->dsLevel= -1;
    ds->dsAutoupd= 0;
    ds->dsHidden= 0;
    ds->dsLocked= 0;
    ds->dsSemiHidden= 0;
    ds->dsPersonal= 0;
    ds->dsBusy= 0;

    docInitSectionProperties( &(ds->dsSectionProperties) );
    docInitParagraphProperties( &(ds->dsParagraphProperties) );
    utilInitTextAttribute( &(ds->dsTextAttribute) );

    PROPmaskCLEAR( &(ds->dsSectMask ) );
    PROPmaskCLEAR( &(ds->dsParaMask ) );
    PROPmaskCLEAR( &(ds->dsTextMask ) );

    ds->dsName= (unsigned char *)0;

    return;
    }

void docCleanDocumentStyle(	DocumentStyle *	ds )
    {
    docCleanParagraphProperties( &(ds->dsParagraphProperties) );
    docCleanSectionProperties( &(ds->dsSectionProperties) );

    if  ( ds->dsName )
	{ free( ds->dsName );	}

    return;
    }

int docCopyStyle(	DocumentStyle *		to,
			const DocumentStyle *	from )
    {
    DocumentStyle	ds;
    PropertyMask	ppChgMask;

    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    docInitDocumentStyle( &ds );
    ds= *from;

    docInitParagraphProperties( &(ds.dsParagraphProperties) );
    docInitSectionProperties( &(ds.dsSectionProperties) );
    ds.dsName= (unsigned char *)0;

    PROPmaskCLEAR( &ppChgMask );

    if  ( docUpdParaProperties( &ppChgMask, &(ds.dsParagraphProperties),
		    &(from->dsParaMask), &(from->dsParagraphProperties),
		    colorMap, listStyleMap ) )
	{ docCleanDocumentStyle( &ds ); return -1;	}

    if  ( docCopySectionProperties( &(ds.dsSectionProperties),
					&(from->dsSectionProperties ) ) )
	{ docCleanDocumentStyle( &ds ); return -1;	}

    if  ( from->dsName )
	{
	ds.dsName= (unsigned char *)strdup( (char *)from->dsName );
	if  ( ! ds.dsName )
	    { XDEB(ds.dsName); docCleanDocumentStyle( &ds ); return -1;	}
	}

    docCleanDocumentStyle( to );
    *to= ds;

    return 0;
    }

DocumentStyle * docInsertStyle(	DocumentStyleSheet *	dss,
				int			n )
    {
    DocumentStyle *	ds;

    if  ( n >= dss->dssStyleCount )
	{
	ds= (DocumentStyle *)realloc( dss->dssStyles,
				    ( n + 1 ) * sizeof( DocumentStyle ) );
	}
    else{
	ds= (DocumentStyle *)realloc( dss->dssStyles,
		    ( dss->dssStyleCount + 1 ) * sizeof( DocumentStyle ) );
	}
    if  ( ! ds )
	{ LLDEB(dss->dssStyleCount,ds); return (DocumentStyle *)0; }
    dss->dssStyles= ds;

    if  ( n == -1 )
	{ n= dss->dssStyleCount;	}
    else{
	if  ( n < dss->dssStyleCount && ds[n].dsStyleNumber >= 0 )
	    { LLDEB(n,ds[n].dsStyleNumber);	}
	}

    while( dss->dssStyleCount < n )
	{ docInitDocumentStyle( ds+ dss->dssStyleCount ); dss->dssStyleCount++;	}

    ds += n;
    docInitDocumentStyle( ds );

    ds->dsStyleNumber= n;
    ds->dsName= (unsigned char *)0;

    if  ( n >= dss->dssStyleCount )
	{ dss->dssStyleCount= n+ 1;	}

    return ds;
    }

void docCleanStyleSheet(	DocumentStyleSheet *	dss )
    {
    DocumentStyle *	ds;
    int			i;

    ds= dss->dssStyles;
    for ( i= 0; i < dss->dssStyleCount; ds++, i++ )
	{ docCleanDocumentStyle( ds ); }

    if  ( dss->dssStyles )
	{ free( dss->dssStyles );	}

    return;
    }

void docInitStyleSheet(	DocumentStyleSheet *	dss )
    {
    dss->dssStyles= (DocumentStyle *)0;
    dss->dssStyleCount= 0;
    }

