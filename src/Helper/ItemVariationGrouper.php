<?php

namespace ElasticExport\Helper;

use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\Item\DataLayer\Models\Record;

class ItemVariationGrouper
{
    /**
     * RecordList $list
     */
    private $list;


    /**
     * ItemVariationGrouper constructor.
     * @param RecordList $list
     */
    public function __construct(RecordList $list)
    {
        $this->list = $list;
    }

    /**
     * Check whether $list contains more entries
     * @return bool
     */
    public function hasNext():bool
    {
        if ($this->list instanceof RecordList)
        {
            if($this->list->count() > 0 &&
            $this->list->valid())
            return true;
        }
        return false;
    }

    /**
     * returns a group of variations with same itemId
     * @return array
     */
    public function getNextGroup():array
    {
        $variationList = array();
        $currentItemId = null;

        while($this->hasNext())
        {
            $variation = $this->list->current();
            if(!($variation instanceof Record))
            {
                break;
            }

            if (is_null($currentItemId))
            {
                $currentItemId = $variation->itemBase->id;
            }

            $this->list->next();
            if($variation->variationBase->primaryVariation === true)
            {
                array_unshift($variationList, $variation);
            }
            else
            {
                array_push($variationList, $variation);
            }

            if($this->hasNext())
            {
                $nextVariation = $this->list->current();
                if(	$nextVariation instanceof Record &&
                    $nextVariation->itemBase->id != $currentItemId)
                {
                    break;
                }

            }
        }

        return $variationList;
    }
}
