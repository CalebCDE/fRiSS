#include "fr_def.h"
#include "xmlnode.h"
#include "fr_fstringitem.h"

#if 0
	#define PPRINT(x) printf x
	#define PPUTS(x) puts(x)
#else
	#define PPRINT(x)
	#define PPUTS(x)
#endif

/*	Extracts the CHANNEL and ITEM-information from XML-Tree into 
 *	a BList of XmlNodes.
 *
 *	XmlNode* root		the tree
 *	BList* list			the target list
 *	BString& status		a good place for error messages
 *
 *	returns
 *		<0				on error
 *		0				no items in tree (sort of error)
 *		>0				items in list
 */
int Parse_rss( XmlNode* root, BList* list, BString& status, bool& updatesFeedList, bool addDesc)
{
	PPUTS("BEGIN PARSE_RSS");
	XmlNode* channel = root->FindChild( "channel", NULL, true );
	
	if (!channel) {
		status = "Error: Invalid File. File does not have <channel>";
		PPUTS("ERROR PARSE_RSS: can't find channel");
		return -1;
	}
	
	/*
	puts("===============");
	root->Display();
	puts("===============");
	*/
	
	FStringItem* fi;
	
	BString ChanLink;
	bool	hasChanLink = false;

	{
		fi = new FStringItem;
		
		XmlNode* p;
		p = channel->FindChild("title");
		if (p)
			fi->SetText(p->Value());
		else
			PPUTS("Channel has no title");
		
		p = channel->FindChild("link");
		if (p) {
			fi->SetUrl(p->Value());
			ChanLink = p->Value();
			hasChanLink = true;
		}
		else
			PPUTS("Channel has no link");
			
		p = channel->FindChild("description");
		if (p)
			fi->SetDesc(p->Value());
		else
			PPUTS("Channel has no descrtion");
	
		list->AddItem(fi);
		PPUTS("");
	}
	
	PPRINT(( "\nNUN DIE ITEMS!\n" ));
	
	XmlNode* item = root->FindChild("item", NULL, true);
	if (!item) {
		PPUTS("File has no items");
		return -1;
	}
	
	PPRINT(("Found: %s = %i\n", item->Name(), item->Type() ));
	
	// parent is at least root
	XmlNode* items = item->Parent();
	XmlNode* p;
	
	int itemcount = 0;
	
	while (item) {
		itemcount++;
		
		fi = new FStringItem;
		
		p = item->FindChild("title");
		if (p) {
			fi->SetText(p->Value());
			PPRINT(( "Item name is '%s'\n", p->Value() ));
		}
		else
			PPUTS("Item has no title");
		
		p = item->FindChild("link");
		if (p) {
			fi->SetUrl(p->Value());
			PPRINT(( "Item link is '%s'\n", p->Value() ));
		}
		else {
			if (hasChanLink) {
				fi->SetUrl(ChanLink.String());
				PPUTS("Item has no link, we will use the Channel's link instead!");		
			}
			else
				PPUTS("Item has no link");
		}
		
		p = item->FindChild("description");
		if (p) {
			if (addDesc) {
				BString a(fi->Text());
				a << " : " << p->Value();
				fi->SetText(a.String());
				fi->SetDesc(p->Value());
			}
			else
				fi->SetDesc(p->Value());
		}
		else
			PPUTS("Item has no description");	
	
		list->AddItem(fi);
		
		PPUTS("");
		
		// get next item:
		item = items->FindChild("item", item);
	}
	
	updatesFeedList = true;

	PPUTS("END OF PARSE_RSS");	
	return itemcount;
}
