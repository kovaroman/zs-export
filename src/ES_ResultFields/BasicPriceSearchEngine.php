<?php

namespace ElasticExport\ES_ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Search\Mutators\ImageMutator;
use Plenty\Modules\Cloud\ElasticSearch\Lib\Source\Mutator\BuiltIn\LanguageMutator;
use Plenty\Modules\Item\Search\Mutators\DefaultCategoryMutator;

/**
 * Class BasicPriceSearchEngine
 */
class BasicPriceSearchEngine extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

    /**
     * BasicPriceSearchEngine constructor.
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
        $this->arrayHelper = $arrayHelper;
    }

    /**
     * Generate result fields.
     * @param  array $formatSettings = []
     * @return array
     */
    public function generateResultFields(array $formatSettings = []):array
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

        $reference = $settings->get('referrerId') ? $settings->get('referrerId') : -1;

        //Mutator
        /**
         * @var ImageMutator $imageMutator
         */
        $imageMutator = pluginApp(ImageMutator::class);
        $imageMutator->addMarket($reference);
        /**
         * @var LanguageMutator $languageMutator
         */
        $languageMutator = pluginApp(LanguageMutator::class, [[$settings->get('lang')]]);
        /**
         * @var DefaultCategoryMutator $defaultCategoryMutator
         */
        $defaultCategoryMutator = pluginApp(DefaultCategoryMutator::class);
        $defaultCategoryMutator->setPlentyId($settings->get('plentyId'));

        $fields = [
            [
                //item
                'item.id',
                'item.manufacturer.id',
                'item.amazonFedas',

                //variation
                'id',
                'variation.availability.id',
                'variation.model',
                'variation.weightG',

                //texts
                'texts.name'.$settings->get('nameId'),
                'texts.shortDescription',
                'texts.description',
                'texts.technicalData',
                'texts.urlPath',

                //unit
                'unit.content',
                'unit.id',

                //images
                'images.item.id',
                'images.item.type',
                'images.item.fileType',
                'images.item.path',
                'images.item.position',
                'images.item.cleanImageName',
                'images.variation.id',
                'images.variation.type',
                'images.variation.fileType',
                'images.variation.path',
                'images.variation.position',
                'images.variation.cleanImageName',

                //defaultCategories
                'defaultCategories.id',

                //barcodes
                'barcodes.code',
                'barcodes.type',
                'barcodes.id',
                'barcodes.name',
            ],

            [
                $imageMutator,
                $languageMutator,
                $defaultCategoryMutator,
            ],
        ];

        return $fields;
    }
}
