<?php

namespace ElasticExport\ES_IDL_ResultList;

use Plenty\Modules\Item\DataLayer\Contracts\ItemDataLayerRepositoryContract;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\DataLayer\Models\RecordList;

/**
 * Class IdealoDE
 * @package ElasticExport\ES_IDL_ResultList
 */
class IdealoDE
{
    const IDEALO_DE = 121;

    /**
     * Creates and retrieves the extra needed data from ItemDataLayer.
     *
     * @param array $variationIds
     * @param KeyValue $settings
     * @return RecordList|string
     */
    public function getResultList($variationIds, $settings)
    {
        if(is_array($variationIds) && count($variationIds) > 0)
        {
            $searchFilter = array(
                'variationBase.hasId' => array(
                    'id' => $variationIds
                )
            );

            $resultFields = array(
                'itemBase' => array(
                    'id',
                ),

                'variationBase' => array(
                    'id',
                    'customNumber',
                ),

                'itemPropertyList' => array(
                    'params' => array(),
                    'fields' => array(
                        'propertyId',
                        'propertyValue',
                    )
                ),

                'variationStock' => array(
                    'params' => array(
                        'type' => 'virtual'
                    ),
                    'fields' => array(
                        'stockNet'
                    )
                ),

                'variationRetailPrice' => array(
                    'params' => array(
                        'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : self::IDEALO_DE,
                    ),
                    'fields' => array(
                        'price',
                        'vatValue',
                    ),
                ),

                'variationRecommendedRetailPrice' => array(
                    'params' => array(
                        'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : self::IDEALO_DE,
                    ),
                    'fields' => array(
                        'price',    // uvp
                    ),
                ),

                'variationSpecialOfferRetailPrice' => array(
                    'params' => array(
                        'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : self::IDEALO_DE,
                    ),
                    'fields' => array(
                        'retailPrice',
                    ),
                ),
            );

            $itemDataLayer = pluginApp(ItemDataLayerRepositoryContract::class);
            /**
             * @var ItemDataLayerRepositoryContract $itemDataLayer
             */
            $itemDataLayer = $itemDataLayer->search($resultFields, $searchFilter);

            return $itemDataLayer;
        }

        return '';
    }
}