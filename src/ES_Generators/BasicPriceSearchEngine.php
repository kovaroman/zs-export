<?php
namespace ElasticExport\ES_Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;

class BasicPriceSearchEngine extends CSVGenerator
{
    const DELIMITER = '	';
    /*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

    /*
     * @var ArrayHelper
     */
    private $arrayHelper;

    /**
     * @var array $idlVariations
     */
    private $idlVariations = array();

    /**
     * BasicPriceSearchEngine constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
    }

    /**
     * @param array $resultData
	 * @param array $formatSettings
     */
    protected function generateContent($resultData, array $formatSettings = [])
	{
		if(is_array($resultData) && count($resultData['documents']) > 0)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(self::DELIMITER);

			$this->addCSVContent([
					'article_id',
                    'deeplink',
                    'name',
                    'short_description',
                    'description',
                    'article_no',
                    'producer',
                    'model',
                    'availability',
                    'ean',
                    'isbn',
                    'fedas',
                    'unit',
                    'price',
                    'price_old',
                    'weight',
                    'category1',
                    'category2',
                    'category3',
                    'category4',
                    'category5',
                    'category6',
                    'category_concat',
                    'image_url_preview',
                    'image_url',
                    'shipment_and_handling',
                    'unit_price',
                    'unit_price_value',
                    'unit_price_lot',
			    ]);

            //Create a List of all VariationIds
            $variationIdList = array();
            foreach($resultData['documents'] as $variation)
            {
                $variationIdList[] = $variation['id'];
            }

            //Get the missing fields in ES from IDL
            if(is_array($variationIdList) && count($variationIdList) > 0)
            {
                /**
                 * @var \ElasticExport\ES_IDL_ResultList\BasicPriceSearchEngine $idlResultList
                 */
                $idlResultList = pluginApp(\ElasticExport\ES_IDL_ResultList\BasicPriceSearchEngine::class);
                $idlResultList = $idlResultList->getResultList($variationIdList, $settings);
            }

            //Creates an array with the variationId as key to surpass the sorting problem
            if(isset($idlResultList) && $idlResultList instanceof RecordList)
            {
                $this->createIdlArray($idlResultList);
            }

			foreach($resultData['documents'] as $item)
			{
                $rrp = $this->elasticExportHelper->getEsRecommendedRetailPrice($this->idlVariations[$item['id']], $settings) > $this->idlVariations[$item['id']]['variationRetailPrice.price']
                    ? $this->elasticExportHelper->getEsRecommendedRetailPrice($this->idlVariations[$item['id']], $settings) : '';

                $basePriceList = $this->elasticExportHelper->getEsBasePriceList($item, (float) $this->idlVariations[$item['id']]['variationRetailPrice.price'], $settings);
                $shipmentAndHandling = $this->elasticExportHelper->getEsShippingCost($item['data']['item']['id'], $settings);
                if(!is_null($shipmentAndHandling))
                {
                    $shipmentAndHandling = number_format((float)$shipmentAndHandling, 2, ',', '');
                }
                else
                {
                    $shipmentAndHandling = '';
                }

				$data = [
					'article_id'            => $item['data']['item']['id'],
                    'deeplink'              => $this->elasticExportHelper->getEsUrl($item, $settings, true, false),
                    'name'                  => $this->elasticExportHelper->getEsName($item, $settings),
                    'short_description'     => $this->elasticExportHelper->getEsPreviewText($item, $settings),
                    'description'           => $this->elasticExportHelper->getEsDescription($item, $settings, 256),
                    'article_no'            => $this->idlVariations[$item['id']]['variationBase.customNumber'],
                    'producer'              => $this->elasticExportHelper->getExternalManufacturerName($item['data']['item']['manufacturer']['id']),
                    'model'                 => $item['data']['variation']['model'],
                    'availability'          => $this->elasticExportHelper->getEsAvailability($item, $settings),
                    'ean'                   => $this->elasticExportHelper->getEsBarcodeByType($item, ElasticExportHelper::BARCODE_EAN),
                    'isbn'                  => $this->elasticExportHelper->getEsBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
                    'fedas'                 => $item['data']['item']['amazonFedas'],
                    'unit'                  => $basePriceList['unit'],
                    'price'                 => number_format((float)$this->idlVariations[$item['id']]['variationRetailPrice.price'], 2, '.', ''),
                    'price_old'             => $rrp,
                    'weight'                => $item['data']['variation']['weightG'],
                    'category1'             => $this->elasticExportHelper->getEsCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 1),
                    'category2'             => $this->elasticExportHelper->getEsCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 2),
                    'category3'             => $this->elasticExportHelper->getEsCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 3),
                    'category4'             => $this->elasticExportHelper->getEsCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 4),
                    'category5'             => $this->elasticExportHelper->getEsCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 5),
                    'category6'             => $this->elasticExportHelper->getEsCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 6),
                    'category_concat'       => $this->elasticExportHelper->getCategory((int)$item['data']['defaultCategories'][0]['id'], $settings->get('lang'), $settings->get('plentyId')),
                    'image_url_preview'     => $this->getImages($item, $settings, ';', 'preview'),
                    'image_url'             => $this->elasticExportHelper->getEsMainImage($item, $settings),
                    'shipment_and_handling' => $shipmentAndHandling,
                    'unit_price'            => $this->elasticExportHelper->getEsBasePrice($item, $this->idlVariations),
                    'unit_price_value'      => $basePriceList['price'],
                    'unit_price_lot'        => $basePriceList['lot']
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Get images.
     * @param  array   $item
     * @param  KeyValue $settings
     * @param  string   $separator  = ','
     * @param  string   $imageType  = 'normal'
     * @return string
     */
     public function getImages($item, KeyValue $settings, string $separator = ',', string $imageType = 'normal'):string
     {
         $list = $this->elasticExportHelper->getEsImageList($item, $settings, $imageType);

         if(count($list))
         {
             return implode($separator, $list);
         }

         return '';
     }

    /**
     * @param RecordList $idlResultList
     */
    private function createIdlArray($idlResultList)
    {
        if($idlResultList instanceof RecordList)
        {
            foreach($idlResultList as $idlVariation)
            {
                if($idlVariation instanceof Record)
                {
                    $this->idlVariations[$idlVariation->variationBase->id] = [
                        'itemBase.id' => $idlVariation->itemBase->id,
                        'variationBase.id' => $idlVariation->variationBase->id,
                        'variationBase.customNumber' => $idlVariation->variationBase->customNumber,
                        'variationRetailPrice.price' => $idlVariation->variationRetailPrice->price,
                        'variationRecommendedRetailPrice.price' => $idlVariation->variationRecommendedRetailPrice->price,
                    ];
                }
            }
        }
    }
}
