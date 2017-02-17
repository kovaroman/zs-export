<?php
namespace ElasticExport\Generators;

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
     * @param RecordList $resultData
	 * @param array $formatSettings
     */
    protected function generateContent( $resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
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

			foreach($resultData as $item)
			{
                $rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : '';

                $basePriceList = $this->elasticExportHelper->getBasePriceList($item, $settings);
                $shipmentAndHandling = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(!is_null($shipmentAndHandling))
                {
                    $shipmentAndHandling = number_format((float)$shipmentAndHandling, 2, ',', '');
                }
                else
                {
                    $shipmentAndHandling = '';
                }

				$data = [
					'article_id'            => $item->itemBase->id,
                    'deeplink'              => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'name'                  => $this->elasticExportHelper->getName($item, $settings),
                    'short_description'     => $this->elasticExportHelper->getPreviewText($item, $settings),
                    'description'           => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'article_no'            => $item->variationBase->customNumber,
                    'producer'              => $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId),
                    'model'                 => $item->variationBase->model,
                    'availability'          => $this->elasticExportHelper->getAvailability($item, $settings),
                    'ean'                   => $this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_EAN),
                    'isbn'                  => $this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
                    'fedas'                 => $item->itemBase->fedas,
                    'unit'                  => $basePriceList['unit'],
                    'price'                 => number_format((float)$this->elasticExportHelper->getPrice($item), 2, '.', ''),
                    'price_old'             => $rrp,
                    'weight'                => $item->variationBase->weightG,
                    'category1'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 1),
                    'category2'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 2),
                    'category3'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 3),
                    'category4'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 4),
                    'category5'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 5),
                    'category6'             => $this->elasticExportHelper->getCategoryBranch($item, $settings, 6),
                    'category_concat'       => $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'image_url_preview'     => $this->getImages($item, $settings, ';', 'preview'),
                    'image_url'             => $this->elasticExportHelper->getMainImage($item, $settings),
                    'shipment_and_handling' => $shipmentAndHandling,
                    'unit_price'            => $this->elasticExportHelper->getBasePrice($item, $settings),
                    'unit_price_value'      => $basePriceList['price'],
                    'unit_price_lot'        => $basePriceList['lot']
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Get images.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator  = ','
     * @param  string   $imageType  = 'normal'
     * @return string
     */
     public function getImages(Record $item, KeyValue $settings, string $separator = ',', string $imageType = 'normal'):string
     {
         $list = $this->elasticExportHelper->getImageList($item, $settings, $imageType);

         if(count($list))
         {
             return implode($separator, $list);
         }

         return '';
     }
}
