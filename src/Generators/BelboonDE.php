<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Contracts\UrlBuilderRepositoryContract;
use Plenty\Modules\Helper\Models\KeyValue;

class BelboonDE extends CSVGenerator
{
	const DELIMITER = ';';

    const IMAGE_SIZE_WIDTH = 'width';
    const IMAGE_SIZE_HEIGHT = 'height';

    /*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

  	/*
  	 * @var ArrayHelper
  	 */
  	private $arrayHelper;

	  /**
     * BelboonDE constructor.
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
	 */
	protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(self::DELIMITER);

			$this->addCSVContent([
                'Merchant_ProductNumber',
                'EAN_Code',
                'Product_Title',
                'Brand',
                'Price',
                'Currency',
                'DeepLink_URL',
                'Image_Small_URL',
                'Image_Small_WIDTH',
                'Image_Small_HEIGHT',
                'Image_Large_URL',
                'Image_Large_WIDTH',
                'Image_Large_HEIGHT',
                'Merchant_Product_Category',
                'Keywords',
                'Product_Description_Short',
                'Product_Description_Long',
                'Shipping',
                'Availability',
                'Unit_Price',
			]);

			foreach($resultData as $item)
			{
				$previewImageInformation = $this->getImageInformation($item, $settings, 'preview');
				$largeImageInformation = $this->getImageInformation($item, $settings, 'normal');

                $shipping = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(!is_null($shipping))
                {
                    $shipping = number_format($shipping, 2, ',', '');
                }
                else
                {
                    $shipping = '';
                }

		        $data = [
		            'Merchant_ProductNumber'      => $item->itemBase->id,
		            'EAN_Code'                    => $item->variationBarcode->code,
		            'Product_Title'               => $this->elasticExportHelper->getName($item, $settings, 256),
		            'Brand'                       => $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId),
		            'Price'                       => number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
		            'Currency'                    => $item->variationRetailPrice->currency,
		            'DeepLink_URL'                => $this->elasticExportHelper->getUrl($item, $settings),
		            'Image_Small_URL'             => $previewImageInformation['url'],
		            'Image_Small_WIDTH'           => $previewImageInformation['width'],
		            'Image_Small_HEIGHT'          => $previewImageInformation['height'],
		            'Image_Large_URL'             => $largeImageInformation['url'],
		            'Image_Large_WIDTH'           => $largeImageInformation['width'],
		            'Image_Large_HEIGHT'          => $largeImageInformation['height'],
		            'Merchant_Product_Category'   => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
		            'Keywords'                    => $item->itemDescription->keywords,
		            'Product_Description_Short'   => $this->elasticExportHelper->getPreviewText($item, $settings, 256),
		            'Product_Description_Long'    => $this->elasticExportHelper->getDescription($item, $settings, 256),
		            'Shipping'                    => $shipping,
		            'Availability'                => $this->elasticExportHelper->getAvailability($item, $settings, false),
		            'Unit_Price'                  => $this->elasticExportHelper->getBasePrice($item, $settings),
		        ];

		        $this->addCSVContent(array_values($data));
			}
		}
	}

	private function getImageInformation(Record $item, KeyValue $settings, string $imageType):array
	{
		$imageList = $this->elasticExportHelper->getImageList($item, $settings, $imageType);

		if(count($imageList) > 0)
		{
			$result = getimagesize($imageList[0]);
			$imageInformation = [
				'url' => $imageList[0],
				'width' => (int)$result[0],
				'height' => (int)$result[1],
			];
		}
		else
		{
			$imageInformation = [
				'url' => '',
				'width' => '',
				'height' => '',
			];
		}

		return $imageInformation;
	}
}
